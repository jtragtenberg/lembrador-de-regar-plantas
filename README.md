# 🌱 Lembrador de Regar Plantas

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

## Hardware

- [M5Stack Core](https://docs.m5stack.com/en/core/basic) (modelo preto/básico)
- Tela LCD 320×240
- 3 botões físicos (A, B, C)

## Setup

1. Instale o [VS Code](https://code.visualstudio.com/) com a extensão [PlatformIO IDE](https://platformio.org/platformio-ide)
2. Clone o repositório e abra a pasta no VS Code
3. Clique em **PlatformIO: Upload** (ou `pio run -t upload`)

O PlatformIO instala automaticamente as dependências (M5Stack library, ESP32 toolchain).

## Dependências

- `espressif32 @ 6.9.0` — plataforma Arduino para ESP32
- `m5stack/M5Stack @ ^0.4.6` — biblioteca oficial do M5Stack
