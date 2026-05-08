#include <M5Stack.h>
#include <Preferences.h>

#ifdef M5FIRE
#include <Adafruit_NeoPixel.h>
#define LED_PIN   15
#define LED_COUNT 10
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
#endif

#define C565(r, g, b) \
    ((uint16_t)((((r)&0xF8u) << 8) | (((g)&0xFCu) << 3) | ((b) >> 3)))

// ── Plant config ──────────────────────────────────────────────────────────────
static const int      NUM_PLANTS = 3;
static const uint32_t INTERVAL_SEC[NUM_PLANTS] = {
    3  * 24u * 3600u,
    7  * 24u * 3600u,
    14 * 24u * 3600u,
};
static const char* PLANT_NAME[NUM_PLANTS] = { "Jiboia", "Jib.Agua", "Cactus" };
static const char* PREF_KEY[NUM_PLANTS]   = { "e0", "e1", "e2" };

// ── Display layout ────────────────────────────────────────────────────────────
static const int SCR_H  = 240;
static const int SEC_W  = 106;
static const int DIV_W  = 1;

// ── Palette ───────────────────────────────────────────────────────────────────
static const uint16_t C_BG      = C565( 10,  10,  20);
static const uint16_t C_WATER   = C565( 25, 100, 220);
static const uint16_t C_ALARM_A = C565(220,  20,  20);
static const uint16_t C_ALARM_B = C565( 80,   0,   0);
static const uint16_t C_GREEN   = C565( 34, 139,  34);
static const uint16_t C_LTGREEN = C565(100, 200,  80);
static const uint16_t C_BROWN   = C565(101,  67,  33);
static const uint16_t C_WHITE   = 0xFFFFu;
static const uint16_t C_DIVIDER = C565( 60,  60,  60);
static const uint16_t C_GLASS   = C565(140, 200, 255);
static const uint16_t C_VSWATER = C565( 50, 150, 255);

// ── State ─────────────────────────────────────────────────────────────────────
Preferences  prefs;
TFT_eSprite  spr(&M5.Lcd);   // sprite único reutilizado para cada seção
uint32_t elapsedSec[NUM_PLANTS];
bool     blinkState  = false;
uint32_t lastBlink   = 0;
uint32_t lastSecTick = 0;
uint32_t lastSave    = 0;

inline int sectionX(int i) { return i * (SEC_W + DIV_W); }

// ── Plant drawings (coordenadas locais ao sprite 106×240) ─────────────────────

void drawPothos(int oy) {
    int cx = SEC_W / 2;
    spr.drawLine(cx, oy + 65, cx,      oy + 45, C_GREEN);
    spr.drawLine(cx, oy + 45, cx - 12, oy + 22, C_GREEN);
    spr.drawLine(cx, oy + 45, cx + 10, oy + 25, C_GREEN);
    spr.drawLine(cx, oy + 55, cx - 14, oy + 40, C_GREEN);
    spr.fillEllipse(cx - 18, oy + 19, 15, 9,  C_GREEN);
    spr.fillEllipse(cx + 14, oy + 21, 13, 8,  C_GREEN);
    spr.fillEllipse(cx - 20, oy + 36, 12, 7,  C_LTGREEN);
    spr.fillEllipse(cx +  8, oy + 12, 10, 6,  C_LTGREEN);
    spr.fillRect    (cx - 14, oy + 65, 28, 5,  C_BROWN);
    spr.fillTriangle(cx - 14, oy + 70, cx + 14, oy + 70, cx, oy + 88, C_BROWN);
}

void drawWaterPlant(int oy) {
    int cx = SEC_W / 2;
    spr.drawLine(cx, oy + 62, cx,      oy + 40, C_GREEN);
    spr.drawLine(cx, oy + 40, cx - 10, oy + 20, C_GREEN);
    spr.drawLine(cx, oy + 40, cx + 10, oy + 18, C_GREEN);
    spr.drawLine(cx, oy + 50, cx - 12, oy + 34, C_GREEN);
    spr.fillEllipse(cx - 16, oy + 17, 13, 8, C_GREEN);
    spr.fillEllipse(cx + 14, oy + 14, 12, 7, C_GREEN);
    spr.fillEllipse(cx - 17, oy + 30, 10, 6, C_LTGREEN);
    spr.drawRect(cx - 16, oy + 60, 32, 32, C_GLASS);
    spr.drawRect(cx - 15, oy + 61, 30, 30, C_GLASS);
    spr.fillRect(cx - 15, oy + 74, 30, 16, C_VSWATER);
    spr.drawLine(cx,     oy + 62, cx - 5, oy + 82, C_LTGREEN);
    spr.drawLine(cx,     oy + 62, cx + 5, oy + 80, C_LTGREEN);
    spr.drawLine(cx - 3, oy + 68, cx - 9, oy + 85, C_LTGREEN);
}

void drawCactus(int oy) {
    int cx = SEC_W / 2;
    spr.fillRoundRect(cx - 8,  oy +  8, 16, 55, 5, C_GREEN);
    spr.fillRoundRect(cx - 26, oy + 22, 20, 10, 4, C_GREEN);
    spr.fillRoundRect(cx - 26, oy + 12, 10, 22, 4, C_GREEN);
    spr.fillRoundRect(cx +  6, oy + 30, 20, 10, 4, C_GREEN);
    spr.fillRoundRect(cx + 16, oy + 20, 10, 22, 4, C_GREEN);
    spr.fillCircle(cx,      oy + 14, 2, C_LTGREEN);
    spr.fillCircle(cx - 4,  oy + 28, 2, C_LTGREEN);
    spr.fillCircle(cx + 4,  oy + 42, 2, C_LTGREEN);
    spr.fillCircle(cx,      oy + 56, 2, C_LTGREEN);
    spr.fillRect    (cx - 14, oy + 63, 28, 6,  C_BROWN);
    spr.fillTriangle(cx - 14, oy + 69, cx + 14, oy + 69, cx, oy + 88, C_BROWN);
}

// ── Renderiza seção no sprite e empurra para a tela de uma vez ────────────────
void drawSection(int idx) {
    uint32_t elapsed  = elapsedSec[idx];
    uint32_t interval = INTERVAL_SEC[idx];
    bool     alarm    = (elapsed >= interval);

    uint16_t bg = alarm ? (blinkState ? C_ALARM_A : C_ALARM_B) : C_BG;
    spr.fillSprite(bg);

    if (!alarm) {
        float ratio  = 1.0f - (float)elapsed / (float)interval;
        int   waterH = (int)(ratio * SCR_H);
        if (waterH > 0)
            spr.fillRect(0, SCR_H - waterH, SEC_W, waterH, C_WATER);
    }

    int plantY = SCR_H / 2 - 55;
    if      (idx == 0) drawPothos    (plantY);
    else if (idx == 1) drawWaterPlant(plantY);
    else               drawCactus    (plantY);

    spr.setTextSize(1);
    spr.setTextColor(C_WHITE);
    spr.setCursor(4, 4);
    spr.print(PLANT_NAME[idx]);

    spr.setCursor(4, SCR_H - 16);
    if (alarm) {
        spr.print(">> REGAR! <<");
    } else {
        int remaining = (int)interval - (int)elapsed;
        int days = remaining / 86400;
        int hrs  = (remaining % 86400) / 3600;
        if (days > 0) spr.printf("%dd %dh", days, hrs);
        else          spr.printf("%dh", hrs);
    }

    // empurra sprite para a posição correta na tela (sem piscar)
    spr.pushSprite(sectionX(idx), 0);

    // divisor vertical (desenhado direto na tela, fora do sprite)
    if (idx < NUM_PLANTS - 1) {
        int dx = sectionX(idx) + SEC_W;
        M5.Lcd.drawLine(dx, 0, dx, SCR_H, C_DIVIDER);
    }
}

void redrawAll() {
    for (int i = 0; i < NUM_PLANTS; i++) drawSection(i);
}

// ── Watering action ───────────────────────────────────────────────────────────
void waterPlant(int idx) {
    elapsedSec[idx] = 0;
    prefs.putUInt(PREF_KEY[idx], 0);
    drawSection(idx);
#ifdef M5FIRE
    bool anyAlarm = false;
    for (int i = 0; i < NUM_PLANTS; i++)
        if (elapsedSec[i] >= INTERVAL_SEC[i]) { anyAlarm = true; break; }
    if (!anyAlarm) { strip.clear(); strip.show(); }
#endif
}

// ── Setup ─────────────────────────────────────────────────────────────────────
void setup() {
    M5.begin();
    M5.Power.begin();
#ifdef M5FIRE
    strip.begin();
    strip.setBrightness(80);
    strip.clear();
    strip.show();
#endif
    M5.Lcd.fillScreen(C_BG);
    M5.Lcd.setTextFont(1);

    spr.createSprite(SEC_W, SCR_H);
    spr.setTextFont(1);

    prefs.begin("plants", false);
    for (int i = 0; i < NUM_PLANTS; i++)
        elapsedSec[i] = prefs.getUInt(PREF_KEY[i], 0);

    redrawAll();

    uint32_t now = millis();
    lastSecTick  = now;
    lastBlink    = now;
    lastSave     = now;
}

// ── Loop ──────────────────────────────────────────────────────────────────────
void loop() {
    M5.update();

    uint32_t now        = millis();
    bool     needRedraw = false;

    if (M5.BtnA.wasPressed()) { waterPlant(0); return; }
    if (M5.BtnB.wasPressed()) { waterPlant(1); return; }
    if (M5.BtnC.wasPressed()) { waterPlant(2); return; }

    if (now - lastSecTick >= 1000u) {
        lastSecTick += 1000u;
        for (int i = 0; i < NUM_PLANTS; i++) elapsedSec[i]++;
        needRedraw = true;
    }

    if (now - lastBlink >= 500u) {
        lastBlink  = now;
        blinkState = !blinkState;
        bool anyAlarm = false;
        for (int i = 0; i < NUM_PLANTS; i++) {
            if (elapsedSec[i] >= INTERVAL_SEC[i]) { anyAlarm = true; needRedraw = true; break; }
        }
#ifdef M5FIRE
        strip.fill(anyAlarm && blinkState ? strip.Color(255, 0, 0) : 0);
        strip.show();
#endif
    }

    if (needRedraw) redrawAll();

    if (now - lastSave >= 60000u) {
        lastSave = now;
        for (int i = 0; i < NUM_PLANTS; i++)
            prefs.putUInt(PREF_KEY[i], elapsedSec[i]);
    }
}
