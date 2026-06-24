// ---------------- Libraries ----------------
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Audio.h"

// ---------------- OLED ----------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SDA_PIN 8
#define SCL_PIN 9

// ---------------- Joystick ----------------
#define JOY_X 1
#define JOY_Y 2
#define JOY_SW 39

// ---------------- SD ----------------
#define SD_CS   11
#define SD_SCK  12
#define SD_MOSI 13
#define SD_MISO 14

// ---------------- I2S ----------------
#define I2S_LRC  15
#define I2S_DOUT 16
#define I2S_BCLK 17

// ================= Initialization =================
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Audio audio;

// ================= Default Values =================
int volume = 11;          // 0..21
const int MAX_VOLUME = 18; // 85% of the MAX 21 for MP3 
volatile bool showVolumeScreen = false;
volatile uint32_t volumeChangedTime = 0;

bool isPlaying = false;
int currentTrack = 0;
String tracks[100];
int trackCount = 0;

volatile bool joyBtn = false;

// ================= JOYSTICK TASK =================
void JoyTask(void *pvParameters)
{
    while (true)
    {
        int x = analogRead(JOY_X);
        int y = analogRead(JOY_Y);

        // VOL+
        if (x > 3500) {
            volume++;
            if (volume > MAX_VOLUME) volume = MAX_VOLUME;
            showVolumeScreen = true;
            volumeChangedTime = millis();
            vTaskDelay(200);
        }

        // VOL-
        if (x < 500) {
            volume--;
            if (volume < 0) volume = 0;
            showVolumeScreen = true;
            volumeChangedTime = millis();
            vTaskDelay(200);
        }

        // NEXT TRACK
        if (y > 3500) {
            currentTrack++;
            vTaskDelay(300);
        }

        // PREVIOUS TRACK
        if (y < 500) {
            currentTrack--;
            if (currentTrack < 1) currentTrack = 1;
            vTaskDelay(300);
        }

        // PLAY/STOP toggle
        joyBtn = !digitalRead(JOY_SW);

        if (joyBtn) {
            isPlaying = !isPlaying;
            vTaskDelay(300);
        }

        vTaskDelay(pdMS_TO_TICKS(20)); // 50Hz
    }
}

// ================= OLED TASK =================
void OledTask(void *pvParameters)
{
    while (true)
    {
        // Show volume screen for 2 seconds after volume change    
         if(showVolumeScreen)
        {
            if(millis() - volumeChangedTime > 2000)
            {
                showVolumeScreen = false;
            }
        }

        display.clearDisplay();

    if(showVolumeScreen)
    {
        display.setCursor(0,0);
        display.println("VOLUME");

        int width = map(
            volume,
            0,
            MAX_VOLUME,
            0,
            100);

        display.drawRect(
            10,
            25,
            100,
            12,
            SSD1306_WHITE);

        display.fillRect(
            10,
            25,
            width,
            12,
            SSD1306_WHITE);

        display.setCursor(40,45);

        int percent =
            map(volume,
                0,
                MAX_VOLUME,
                0,
                80);

        display.printf("%d%%", percent);
    }
    else
    {
        display.setCursor(0,0);
        display.println("MP3 PLAYER");

        display.setCursor(0,15);

        display.print("Track ");
        display.print(currentTrack + 1);
        display.print("/");
        display.println(trackCount);

        display.setCursor(0,35);

        if(trackCount > 0)
        {
            String name =
                tracks[currentTrack];

            if(name.length() > 20)
            {
                name =
                    name.substring(0,20);
            }

            display.println(name);
        }
        else
        {
            display.println("No tracks");
        }

        display.setCursor(0,55);

        display.println(
            isPlaying ?
            "PLAYING" :
            "STOPPED");
    }

    display.display();
    }
}

// ================= SETUP =================
void setup()
{
    Serial.begin(115200);
    pinMode(JOY_SW, INPUT_PULLUP);
    Wire.begin(SDA_PIN, SCL_PIN);

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("OLED Error");
        while (true);
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    // task FreeRTOS
    xTaskCreatePinnedToCore(JoyTask, "Joystick Task", 2048, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(OledTask, "OLED Task", 4096, NULL, 1, NULL, 0);
}

void loop() {
    // тут немає аудіо, тільки інтерфейс
}

