# PvZ GOTY Injector

> For educational purposes only.

A simple memory trainer for **Plants vs. Zombies GOTY Edition (v1.2)** that hooks into the game process to enable toggleable cheats.

Simply build the script and run the game.

## Features

| Key | Feature |
|-----|---------|
| `F1` | Toggle Infinite Sun |
| `F2` | Toggle Global Zero Cooldown |
| `ESC` | Exit |

## Requirements

- Plants vs. Zombies GOTY Edition (v1.2)
- Windows OS
- Run as **Administrator**

## Build

### Windows (Visual Studio)
Compile as a standard Win32 console application with Unicode enabled.

### Mac (cross-compile)
```bash
x86_64-w64-mingw32-g++ cheat.cpp -o cheat.exe -DUNICODE -D_UNICODE