# PicoKernel

A bare-metal kernel for the Raspberry Pi Pico with a custom ST7789 graphics driver and interactive shell — written entirely in C, no MicroPython, no OS.

![Platform](https://img.shields.io/badge/platform-RP2040-blue)
![Language](https://img.shields.io/badge/language-C-brightgreen)
![Display](https://img.shields.io/badge/display-ST7789%20240x135-orange)
![License](https://img.shields.io/badge/license-MIT-purple)

## What is this?

PicoKernel is a from-scratch bare-metal project for the Raspberry Pi Pico + Pimoroni Pico Display. It talks directly to hardware registers — no abstraction layers, no runtime environment. Just C, SPI, and pixels.

## Features

- Custom ST7789 SPI display driver (240x135, RGB565)
- 8x8 bitmap font renderer
- Interactive UART/USB shell with command parsing
- Weighted emotion-aware RGB LED (ported from original MicroPython face project)
- Fully documented, buildable from scratch on Windows/Linux/macOS

## Hardware

| Component | Details |
|---|---|
| MCU | Raspberry Pi Pico (RP2040) |
| Display | Pimoroni Pico Display (ST7789, 240x135) |
| Interface | SPI0 @ 62.5 MHz |
| Input | USB Serial (via TinyUSB) |

## Pin Map

| Pin | Function |
|---|---|
| GPIO 18 | SPI SCK |
| GPIO 19 | SPI MOSI |
| GPIO 17 | CS |
| GPIO 16 | DC |
| GPIO 20 | Backlight |

## Project Structure

pico-kernel/
├── src/
│   ├── main.c              # Entry point
│   ├── drivers/
│   │   ├── st7789.c        # Display driver
│   │   └── st7789.h
│   ├── kernel/
│   │   ├── shell.c         # Interactive shell
│   │   └── shell.h
│   └── font/
│       └── font8x8.h       # Bitmap font (ASCII 32-126)
├── CMakeLists.txt
└── pico_sdk_import.cmake
## Build Instructions

### Requirements

- ARM GCC Toolchain (`gcc-arm-none-eabi`)
- CMake >= 3.13
- Ninja
- Raspberry Pi Pico SDK

### Windows (via Scoop)

```powershell
scoop install git cmake ninja gcc-arm-none-eabi
git clone https://github.com/raspberrypi/pico-sdk --branch master
cd pico-sdk && git submodule update --init
[System.Environment]::SetEnvironmentVariable("PICO_SDK_PATH", "C:\pico\pico-sdk", "User")
```

### Build

```powershell
git clone https://github.com/SassanidBytes/pico-kernel
cd pico-kernel
mkdir build && cd build
cmake -G Ninja ..
ninja
```

### Flash

Hold BOOTSEL on the Pico, plug in USB, then:

```powershell
copy pico_kernel.uf2 E:\
```

## Shell Commands

Connect via any serial terminal (e.g. PuTTY) at `115200` baud:

| Command | Description |
|---|---|
| `help` | Show available commands |
| `hello` | Greeting + system info |
| `clear` | Clear the screen |
| `color` | RGB color test |
| `info` | Display driver info |

## How it works

The kernel boots directly into `main()` after the RP2040 hardware init. It initializes the ST7789 over SPI, clears the framebuffer, renders boot text using a pixel-by-pixel bitmap font renderer, then enters the shell loop — polling USB serial for input and dispatching commands.

No RTOS. No HAL. No MicroPython. Just registers.

## Author

**SassanidBytes** — [github.com/SassanidBytes](https://github.com/SassanidBytes)





