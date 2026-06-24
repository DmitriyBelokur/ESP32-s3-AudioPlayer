// ---------------- Libraries ----------------
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "Audio.h"

#include "config.h"

// ================= Initialization =================
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); // OLED display object
Audio audio; // Audio object
QueueHandle_t playerQueue; // Queue for player commands
TrackInfo tracks[100]; // Array to hold track information
PlayerState state;  // Structure to hold the current state of the player

// Enum for player commands
enum PlayerCommand{
    CMD_NONE,
    CMD_PLAY,
    CMD_PAUSE,
    CMD_NEXT,
    CMD_PREV,
    CMD_VOL_UP,
    CMD_VOL_DOWN
}; 

void scanMusic();

// ================= JOYSTICK TASK =================
void JoyTask(void *pvParameters){
    PlayerCommand cmd = CMD_NONE;
    while (true){
                
        int x = analogRead(JOY_X);
        int y = analogRead(JOY_Y);

        if (x > 3500) cmd = CMD_VOL_UP;
        else if (x < 500) cmd = CMD_VOL_DOWN;
        else if (y > 3500) cmd = CMD_NEXT;
        else if (y < 500) cmd = CMD_PREV;
        else if (!digitalRead(JOY_SW)) cmd = CMD_PLAY;

        if (cmd != CMD_NONE){
            xQueueSend(playerQueue, &cmd, portMAX_DELAY);
            vTaskDelay(pdMS_TO_TICKS(200));
        }
        vTaskDelay(pdMS_TO_TICKS(20)); // 50Hz
    }
}        

void PlayerControlTask(void *pvParameters){
    PlayerCommand cmd;
    while (true){
        if(xQueueReceive(playerQueue, &cmd, portMAX_DELAY)){
            switch(cmd){
                case CMD_VOL_UP: // VOL+
                    volume++;
                    if (volume > MAX_VOLUME) volume = MAX_VOLUME;
                    showVolumeScreen = true;
                    volumeChangedTime = millis();
                    break;
                case CMD_VOL_DOWN: // VOL-
                    volume--;
                    if (volume < 0) volume = 0;
                    showVolumeScreen = true;
                    volumeChangedTime = millis();
                    break;
                case CMD_NEXT: // NEXT TRACK
                    currentTrack++;
                    if(currentTrack >= trackCount) currentTrack = 0;
                    break;
                case CMD_PREV: // PREVIOUS TRACK
                    currentTrack--;
                    if (currentTrack < 0) currentTrack = 0;
                    break;
                case CMD_PLAY:
                    isPlaying = !isPlaying;
                    break;
                default:
                    break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(20)); // 50Hz
    }
}

// ================= OLED TASK =================
void OledTask(void *pvParameters){
    while (true){
        // Show volume screen for 2 seconds after volume change    
         if(showVolumeScreen){
            if(millis() - volumeChangedTime > 2000){
                showVolumeScreen = false;
            }
        }

        display.clearDisplay();

        if(showVolumeScreen){
            display.setCursor(0,0);
            display.println("VOLUME");

            int width = map(
                volume,
                0,
                MAX_VOLUME,
                0,
                100
            );

            display.drawRect(
                10,
                25,
                100,
                12,
                SSD1306_WHITE
            );

            display.fillRect(
                10,
                25,
                width,
                12,
                SSD1306_WHITE
            );

            display.setCursor(40,45);

            int percent = map(
                volume,
                0,
                MAX_VOLUME,
                0,
                80
            );

            display.printf("%d%%", percent);
        }
        else{
        display.setCursor(0,0);
        display.println("MP3 PLAYER");

        display.setCursor(0,15);

        display.print("Track ");
        display.print(currentTrack + 1);
        display.print("/");
        display.println(trackCount);

        display.setCursor(0,35);

        if(trackCount > 0){
            String name = tracks[currentTrack].title;

            if(name.length() > 20){
                name = name.substring(0,20);
            }

            display.println(name);
        }
        else{
            display.println("No tracks");
        }

        display.setCursor(0,55);

        display.println(
            isPlaying ?
            "PLAYING" :
            "STOPPED");
        }

        display.display();
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// ================= AUDIO TASK =================
void AudioTask(void *pvParameters){
   PlayerCommand cmd;

    while(true){
                
        if(xQueueReceive(playerQueue, &cmd, portMAX_DELAY)){
            switch(cmd){
                case CMD_PLAY:
                    if(state.trackCount > 0){
                        audio.connecttoFS(SD, tracks[state.currentTrack].path.c_str());
                        state.isPlaying = true;
                    }
                    break;
                case CMD_PAUSE:
                    audio.stopSong();
                    state.isPlaying = false;
                    break;
                case CMD_NEXT:
                    state.currentTrack = (state.currentTrack + 1) % state.trackCount;
                    audio.connecttoFS(SD, tracks[state.currentTrack].path.c_str());
                    state.isPlaying = true;
                    break;
                case CMD_PREV:
                    state.currentTrack = (state.currentTrack - 1 + state.trackCount) % state.trackCount;
                    audio.connecttoFS(SD, tracks[state.currentTrack].path.c_str());
                    state.isPlaying = true;
                    break;
                case CMD_VOL_UP:
                    if(state.volume < MAX_VOLUME) state.volume++;
                    audio.setVolume(state.volume);
                    state.showVolumeScreen = true;
                    state.volumeChangedTime = millis();
                    break;
                case CMD_VOL_DOWN:
                    if(state.volume > 0) state.volume--;
                    audio.setVolume(state.volume);
                    state.showVolumeScreen = true;
                    state.volumeChangedTime = millis();
                    break;
                default: break;
            }
        }
        
        if(state.isPlaying) audio.loop();
        vTaskDelay(pdMS_TO_TICKS(1));   
    }
}    

// ================= SETUP =================
void setup(){
    Serial.begin(115200);
    playerQueue = xQueueCreate(10, sizeof(PlayerCommand));

    pinMode(JOY_SW, INPUT_PULLUP);
    Wire.begin(SDA_PIN, SCL_PIN);

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("OLED Error");
        while (true);
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    SPI.begin(
        SD_SCK,
        SD_MISO,
        SD_MOSI,
        SD_CS
    );

    if(!SD.begin(SD_CS)){
        Serial.println("SD ERROR");

        while(true)delay(1000);
    }

    scanMusic();
  
    audio.setPinout(
        I2S_BCLK,
        I2S_LRC,
        I2S_DOUT
    );

    // task FreeRTOS
    xTaskCreatePinnedToCore(JoyTask, "Joystick Task", 2048, NULL, 2, NULL, 0);    
    xTaskCreatePinnedToCore(OledTask, "OLED Task", 4096, NULL, 2, NULL, 0);    
    xTaskCreatePinnedToCore(PlayerControlTask, "Player Control Task", 2048, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(AudioTask, "Audio Task", 8192, NULL, 1, NULL, 1);
}

void loop() {
     
}

void scanMusic(){
    trackCount = 0;

    File dir = SD.open("/Music");

    if(!dir) {
        Serial.println("Music folder not found");
        return;
    }

    while(true){
        File file = dir.openNextFile();

        if(!file) break;

        if(!file.isDirectory()){
            String name = file.name();

            if(name.endsWith(".mp3") ||name.endsWith(".MP3")){
                {tracks[trackCount].path = String("/Music/") + name;

                tracks[trackCount].title = name;

                tracks[trackCount].title.replace(".mp3","");
                tracks[trackCount].title.replace(".MP3","");

                trackCount++;

                if(trackCount >= 100) break;
                }
            }

        file.close();
        }

    dir.close();

    Serial.printf(
        "Tracks found: %d\n",
        trackCount);
    }
}

// Callback
void audio_eof_mp3(const char *info){
    Serial.printf("Track finished: %s\n", info);

    currentTrack++;
    if(currentTrack >= trackCount) currentTrack = 0;

    if(trackCount > 0){
        audio.connecttoFS(SD, tracks[currentTrack].path.c_str());
        isPlaying = true;
    }
}
