// ---------------- OLED ----------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SDA_PIN 8
#define SCL_PIN 9

// ---------------- Joystick ----------------
#define JOY_X 1
#define JOY_Y 2
#define JOY_SW 4

// ---------------- SD ----------------
#define SD_CS   11
#define SD_SCK  12
#define SD_MOSI 13
#define SD_MISO 14

// ---------------- I2S ----------------
#define I2S_LRC  15
#define I2S_DOUT 16
#define I2S_BCLK 17
// ================= Default Values =================
volatile int volume = 11;          // 0..21
volatile const int MAX_VOLUME = 18; // 85% of the MAX 21 for MP3 
volatile bool showVolumeScreen = false;
volatile uint32_t volumeChangedTime = 0;

volatile bool isPlaying = false;
volatile int currentTrack = 0;
volatile int trackCount = 0;
volatile bool joyBtn = false;

// ================= Structure =================
struct TrackInfo{
    String path;
    String title;
};

struct PlayerState {
    int volume = 10;
    int currentTrack = 0;
    int trackCount = 0;
    bool isPlaying = false;
    bool showVolumeScreen = false;
    uint32_t volumeChangedTime = 0;
};