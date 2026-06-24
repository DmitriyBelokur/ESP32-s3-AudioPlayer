# ESP32-S3 Audio Player

A FreeRTOS-based MP3 audio player built around ESP32-S3, featuring a SSD1306 OLED display, MicroSD storage, PCM5102A DAC, TPA3118 amplifier and joystick control.

Designed as a lightweight standalone music player with responsive UI and dedicated audio processing running on a separate CPU core.

---

## Features

### Hardware

* ESP32-S3 Dual-Core MCU
* SSD1306 OLED Display (128×64)
* Analog Joystick Navigation
* MicroSD Storage
* PCM5102A I2S DAC
* TPA3118 Power Amplifier

### Software

* FreeRTOS Multi-Core Architecture
* Independent Audio Processing Task
* MP3 Playback
* Automatic Music Library Scan
* Playlist Support
* Play / Pause
* Next / Previous Track
* Volume Control
* OLED User Interface
* Auto Next Track Playback
* PlatformIO Support

---

# Hardware Connections

## ESP32-S3

Tested with:

* ESP32-S3-DevKitC-1

---

## OLED SSD1306

| OLED | ESP32-S3 |
| ---- | -------- |
| SDA  | GPIO8    |
| SCL  | GPIO9    |
| VCC  | 3.3V     |
| GND  | GND      |

---

## Analog Joystick

| Joystick | ESP32-S3 |
| -------- | -------- |
| VRX      | GPIO1    |
| VRY      | GPIO2    |
| SW       | GPIO4    |
| VCC      | 3.3V     |
| GND      | GND      |

---

## MicroSD Module

| SD Module | ESP32-S3 |
| --------- | -------- |
| CS        | GPIO11   |
| SCK       | GPIO12   |
| MOSI      | GPIO13   |
| MISO      | GPIO14   |
| VCC       | 3.3V     |
| GND       | GND      |

---

## PCM5102A DAC

| PCM5102A | ESP32-S3 |
| -------- | -------- |
| BCK      | GPIO17   |
| LCK      | GPIO15   |
| DIN      | GPIO16   |
| VIN      | 3.3V     |
| GND      | GND      |

---

## TPA3118 Amplifier

| PCM5102A | TPA3118 |
| -------- | ------- |
| LOUT     | LIN     |
| ROUT     | RIN     |
| GND      | GND     |

---

# SD Card Layout

The firmware automatically scans the `/Music` directory during startup.

Example:

```text
SD Card
└── Music
    ├── Metallica.mp3
    ├── ACDC.mp3
    ├── Queen.mp3
    └── Sabaton.mp3
```

Supported file extensions:

```text
.mp3
.MP3
```

---

# User Interface

## Playback Screen

```text
MP3 PLAYER

Track 3/12

Metallica

PLAYING
```

---

## Volume Screen

Appears automatically for 2 seconds after volume adjustment.

```text
VOLUME

██████████░░░░

65%
```

Volume is intentionally limited to **80% maximum output** to reduce clipping and speaker distortion.

---

# Controls

| Action         | Control         |
| -------------- | --------------- |
| Next Track     | Joystick Up     |
| Previous Track | Joystick Down   |
| Volume Up      | Joystick Right  |
| Volume Down    | Joystick Left   |
| Play / Pause   | Joystick Button |

---

# Software Architecture

## Core 0 — User Interface

### JoyTask

Responsibilities:

* Read analog joystick
* Detect button presses
* Generate player commands

### OledTask

Responsibilities:

* OLED rendering
* Display current track
* Display playback state
* Display volume level

---

## Core 1 — Audio Engine

### AudioTask

Responsibilities:

* MP3 decoding
* SD card file access
* PCM5102A audio output
* Volume management
* Track switching

---

## Communication Flow

```text
Joystick
    │
    ▼
Player Queue
    │
    ▼
Audio Task
    │
    ▼
Player State
    │
    ▼
OLED Task
```

---

# Project Structure

```text
.
├── include
├── lib
├── src
│   ├── main.cpp
│   ├── config.h
├── platformio.ini
├── README.md
└── LICENSE
```

---

# Building

## Install PlatformIO

Install PlatformIO Core:

https://platformio.org/install

---

## Install Dependencies

```bash
pio pkg install
```

---

## Build Firmware

```bash
pio run
```

---

## Upload Firmware

```bash
pio run -t upload
```

---

## Open Serial Monitor

```bash
pio device monitor
```

---

# PlatformIO Configuration

```ini
[env:esp32-s3-devkitc-1]
platform = espressif32
board = esp32-s3-devkitc-1
framework = arduino

board_build.usb_cdc_on_boot = true

lib_deps =
    adafruit/Adafruit GFX Library
    adafruit/Adafruit SSD1306
    ESP32-audioI2S
```

---

# Memory Usage

Typical resource usage on ESP32-S3:

| Resource              | Usage       |
| --------------------- | ----------- |
| Flash                 | ~1–2 MB     |
| RAM                   | ~150–250 KB |
| Stack (Audio Task)    | 16 KB       |
| Stack (OLED Task)     | 4 KB        |
| Stack (Joystick Task) | 2 KB        |

---

# Future Roadmap

## Playback

* WAV Support
* FLAC Support
* AAC Support

## User Interface

* Progress Bar
* Playback Time
* Folder Navigation
* Scrolling Track Names

## Audio

* Equalizer
* Bass Boost
* Stereo Balance
* Mute Function

## Connectivity

* Bluetooth Audio Receiver
* Wi-Fi Streaming
* Web Interface
* OTA Updates

---

# Troubleshooting

## OLED Not Detected

Check:

* SDA = GPIO8
* SCL = GPIO9
* I2C Address = 0x3C

---

## SD Card Not Detected

Check:

* FAT32 formatting
* Correct SPI wiring
* `/Music` folder exists

---

## No Sound

Check:

* PCM5102A wiring
* I2S pin configuration
* TPA3118 power supply
* Speaker connections

---

# License

* adafruit/Adafruit GFX Library
* adafruit/Adafruit SSD1306
* ESP32-audioI2S

Copyright (c) 2026

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files to deal in the Software without restriction.