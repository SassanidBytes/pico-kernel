# PicoKernel

A bare-metal kernel for the Raspberry Pi Pico with a custom ST7789 graphics driver, interactive shell, dual-core scheduler, games, and security tools — written entirely in C, no MicroPython, no OS.

![Platform](https://img.shields.io/badge/platform-RP2040-blue)
![Language](https://img.shields.io/badge/language-C-brightgreen)
![Display](https://img.shields.io/badge/display-ST7789%20240x135-orange)
![License](https://img.shields.io/badge/license-MIT-purple)
![Cores](https://img.shields.io/badge/cores-dual--core-red)

## What is this?

PicoKernel is a from-scratch bare-metal project for the Raspberry Pi Pico + Pimoroni Pico Display. It talks directly to hardware registers — no abstraction layers, no runtime environment. Just C, SPI, and pixels.

It boots with a custom splash screen, launches a dual-core scheduler, and drops into an interactive shell with games and security tools.

## Features

- Custom ST7789 SPI display driver (240x135, RGB565)
- 8x8 bitmap font renderer
- Interactive USB shell with command parsing
- Dual-core operation (RP2040 Core0 + Core1)
- Round-robin task scheduler
- Mutex-based display synchronization
- Boot splash screen with custom logo
- Pong game with AI opponent
- Space Invaders with enemy AI
- Hardware Password Manager (XOR encrypted, Flash storage)
- Password add/delete with persistent storage
- 2FA TOTP Token Generator
- System info (RAM/Flash usage)
- Heartbeat indicator (Core1)

## Hardware

| Component | Details |
|---|---|
| MCU | Raspberry Pi Pico (RP2040) |
| Display | Pimoroni Pico Display (ST7789, 240x135) |
| Interface | SPI0 @ 62.5 MHz |
| Input | USB Serial + 4 Hardware Buttons |

## Pin Map

| Pin | Function |
|---|---|
| GPIO 18 | SPI SCK |
| GPIO 19 | SPI MOSI |
| GPIO 17 | CS |
| GPIO 16 | DC |
| GPIO 20 | Backlight |
| GPIO 12 | Button A |
| GPIO 13 | Button B |
| GPIO 14 | Button X |
| GPIO 15 | Button Y |

## Project Structure

pico-kernel/
├── src/
│   ├── main.c                  # Entry point, dual-core launch
│   ├── drivers/
│   │   ├── st7789.c            # Display driver
│   │   └── st7789.h
│   ├── kernel/
│   │   ├── shell.c             # Interactive shell
│   │   ├── shell.h
│   │   ├── mutex.c             # Display mutex
│   │   ├── mutex.h
│   │   ├── pong.c              # Pong game
│   │   ├── pong.h
│   │   ├── invaders.c          # Space Invaders
│   │   ├── invaders.h
│   │   ├── passmanager.c       # Password Manager
│   │   ├── passmanager.h
│   │   ├── totp.c              # 2FA Token Generator
│   │   ├── totp.h
│   │   └── tasks/
│   │       ├── scheduler.c     # Round-robin scheduler
│   │       ├── scheduler.h
│   │       └── task.h
│   └── font/
│       ├── font8x8.h           # Bitmap font (ASCII 32-126)
│       └── logo.h              # Boot splash logo (RGB565)
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
| `tasks` | Task manager (dual-core) |
| `sysinfo` | RAM/Flash/Password usage |
| `pong` | Pong vs AI (A=up, B=down, X+Y=exit) |
| `inv` | Space Invaders (A=left, B=right, X=shoot, Y+A=exit) |
| `pass` | Password Manager (A/B=scroll, X=select, Y=exit) |
| `addpass` | Add new password (saved to Flash) |
| `delpass` | Delete password (A/B=scroll, X=delete, Y=cancel) |
| `2fa` | 2FA TOTP Token Generator (Y=exit) |

## How it works

The kernel boots directly into `main()` after the RP2040 hardware init. Core0 runs the shell and scheduler. Core1 runs a heartbeat indicator independently. A mutex ensures safe display access from both cores simultaneously.

No RTOS. No HAL. No MicroPython. Just registers.

## Author

**SassanidBytes** — [github.com/SassanidBytes](https://github.com/SassanidBytes)




