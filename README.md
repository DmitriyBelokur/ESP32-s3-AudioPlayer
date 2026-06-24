# AudioPlayer

FreeRTOS-based multimedia controller for ESP32-S3.

## Features

* ESP32-S3
* SSD1306 OLED Display (128x64)
* Analog Joystick Navigation
* FreeRTOS Multi-Core Architecture
* UI Task
* Input Task
* Event Queue System
* Future Audio Player Support
* PlatformIO Compatible

---

## Hardware

### ESP32-S3

Tested with:

* ESP32-S3-DevKitC-1

---

### OLED Display

SSD1306 128x64 I2C

| OLED | ESP32-S3 |
| ---- | -------- |
| SDA  | GPIO8    |
| SCL  | GPIO9    |
| VCC  | 3.3V     |
| GND  | GND      |

---

### Joystick

| Joystick | ESP32-S3 |
| -------- | -------- |
| VRX      | GPIO1    |
| VRY      | GPIO2    |
| SW       | GPIO4    |
| VCC      | 3.3V     |
| GND      | GND      |

---

## Software Architecture

### Core 0

UI Task

Responsible for:

* Display rendering
* Menu navigation
* Status screens

---

### Core 1

Input Task

Responsible for:

* Joystick reading
* Button processing
* Event generation

---

### FreeRTOS Queue

Communication between tasks is performed using FreeRTOS Queue.

Input Task
↓
Queue
↓
UI Task

---

## Project Structure

```text
.
├── include
├── lib
├── src
│   ├── main.cpp
│   ├── config.h
├── platformio.ini
└── README.md
```

---

## Menu Controls

Joystick Up

* Next Item

Joystick Down

* Previous Item

Joystick Left

* Back

Joystick Right

* Enter

Joystick Button

* Select

---

## PlatformIO

Install dependencies:

```bash
pio pkg install
```

Build:

```bash
pio run
```

Upload:

```bash
pio run -t upload
```

Monitor:

```bash
pio device monitor
```

---

## Future Roadmap

* PCM5102A Audio DAC
* TPA3118 Amplifier
* MicroSD Support
* MP3 Playback
* WAV Playback
* Playlist Support
* Volume Control
* Equalizer
* Bluetooth Audio
* WiFi Streaming

---

## License

MIT License
* adafruit/Adafruit GFX Library
* adafruit/Adafruit SSD1306
* ESP32-audioI2S
```
```
