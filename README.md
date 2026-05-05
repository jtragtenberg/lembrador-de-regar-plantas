# Lembrador de Regar Plantas

Projeto PlatformIO para o **M5Stack Core** (preto) que serve como lembrete visual para regar plantas, com barras de água animadas e alertas de alarme.

## Como funciona

A tela é dividida em 3 colunas, uma para cada planta. Cada coluna exibe uma barra azul que vai descendo conforme o tempo passa, representando o nível de água. Quando o tempo esgota, o fundo pisca em vermelho indicando que é hora de regar.

Apertar o botão correspondente registra a rega e enche a barra novamente.

| Botão | Planta | Intervalo |
|-------|--------|-----------|
| A (esquerdo) | Jibóia / Suculentas | 3 dias |
| B (meio) | Jibóia na água | 1 semana |
| C (direito) | Cactus | 2 semanas |

O tempo decorrido é salvo na flash a cada 60 segundos, então o dispositivo lembra o estado mesmo após desligar.

## Hardware necessário

- [M5Stack Core](https://docs.m5stack.com/en/core/basic) (modelo preto/básico)
- Cabo USB-C

---

## Passo a passo para replicar

### 1. Instale o VS Code

Baixe e instale o [Visual Studio Code](https://code.visualstudio.com/) para o seu sistema operacional.

### 2. Instale a extensão PlatformIO

1. Abra o VS Code
2. Clique no ícone de **Extensões** na barra lateral (ou `Ctrl+Shift+X` / `Cmd+Shift+X`)
3. Pesquise por `PlatformIO IDE`
4. Clique em **Install**
5. Aguarde a instalação terminar e **reinicie o VS Code** quando solicitado

### 3. Clone o repositório

Abra um terminal e execute:

```bash
git clone https://github.com/jtragtenberg/lembrador-de-regar-plantas.git
cd lembrador-de-regar-plantas
```

Ou baixe o ZIP pelo GitHub clicando em **Code → Download ZIP** e descompacte.

### 4. Abra o projeto no VS Code

No terminal, dentro da pasta do projeto:

```bash
code .
```

Ou pelo VS Code: **File → Open Folder** e selecione a pasta `lembrador-de-regar-plantas`.

O PlatformIO vai detectar o `platformio.ini` automaticamente e instalar as dependências (ESP32 toolchain e biblioteca M5Stack) na primeira vez — isso pode levar alguns minutos.

### 5. Conecte o M5Stack

1. Conecte o M5Stack ao computador com o cabo USB-C
2. Ligue o dispositivo (botão lateral esquerdo)
3. O sistema operacional deve reconhecê-lo automaticamente como uma porta serial

**No macOS/Linux:** a porta aparece como `/dev/tty.usbserial-XXXX` ou `/dev/ttyUSB0`  
**No Windows:** aparece como `COM3`, `COM4`, etc. no Gerenciador de Dispositivos

> Se a porta não aparecer, instale o driver CP210x: [Silicon Labs CP210x Driver](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)

### 6. Configure a porta serial (se necessário)

Na maioria dos casos o PlatformIO detecta a porta automaticamente. Caso não detecte, edite o `platformio.ini` e adicione a linha com a porta correta:

```ini
upload_port = /dev/tty.usbserial-XXXX   ; macOS/Linux
; upload_port = COM4                    ; Windows
```

Para descobrir a porta correta:
- **macOS/Linux:** `ls /dev/tty.*` no terminal antes e depois de conectar o cabo
- **Windows:** Gerenciador de Dispositivos → Portas (COM e LPT)

### 7. Faça o upload do código

1. Na barra lateral do VS Code, clique no ícone do **PlatformIO** (alienígena)
2. Em **PROJECT TASKS → m5stack-core-esp32**, clique em **Upload**
3. Aguarde a compilação e o upload (a barra de progresso aparece no terminal)
4. Quando aparecer `SUCCESS`, o M5Stack reinicia automaticamente com o programa rodando

---

## Dependências

- `espressif32 @ 6.9.0` — plataforma Arduino para ESP32
- `m5stack/M5Stack @ ^0.4.6` — biblioteca oficial do M5Stack
