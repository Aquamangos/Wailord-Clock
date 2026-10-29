#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <WiFi.h>
#include <time.h>

const char* ssid = "WIFI-NAME";
const char* wifipassword = "wifi-pass";

const char* ntpServer = "pool.ntp.org";
const char* timeZone = "PST8PDT,M3.2.0,M11.1.0";

#define TFT_SCLK D9 // SCL on screen
#define TFT_MOSI D10 // SDA on screen
#define TFT_RST D8
#define TFT_DC D4
#define TFT_CS D5
#define TFT_BL D6

#define BUZZER_PIN D7

#define SW_1 D0
#define SW_2 D1
#define SW_3 D2
#define SW_4 D3

// fix the SetColRowStart()
class MyST7789 : public Adafruit_ST7789 {
public:
  MyST7789(int8_t cs, int8_t dc, int8_t mosi, int8_t sclk, int8_t rst)
    : Adafruit_ST7789(cs, dc, mosi, sclk, rst) {}
  void setOffsets(uint8_t col, uint8_t row) {
    _colstart = _colstart2 = col;
    _rowstart = _rowstart2 = row;
  }
};

MyST7789 tft(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

bool clockReady = false;
bool use24Hour = false;

void wificonnection() {

    tft.setTextSize(2);
    tft.print("Connecting.....");
    WiFi.begin(ssid, wifipassword);
    while (WiFi.status() != WL_CONNECTED) {
        delay(300);
        tft.print("Connected");
    }
    configTzTime(timeZone, ntpServer);
}

void clearscreen() {

    tft.fillScreen(ST77XX_BLACK);

}


void buzzerbeep() {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(1000);
    digitalWrite(BUZZER_PIN, LOW);
    delay(200);
    
}

void drawtime(){

    struct tm timeInfo;

    if(!getLocalTime(&timeInfo)) {
        tft.setCursor(0,0);
        tft.println("Fetching time...");
        return;
    }

    char timeText[16];

    if (use24Hour) {
        strftime(timeText, sizeof(timeText),
                "%H:%M:%S", &timeInfo);
    } else {
        strftime(timeText, sizeof(timeText),
                "%I:%M:%S %p", &timeInfo);
    }   
    clearscreen();
    tft.setCursor(5,20);
    tft.setTextSize(3);
    tft.setTextColor(ST77XX_WHITE);
    tft.println(timeText);
}

bool buttonPressed(uint8_t pin) {
    if (digitalRead(pin) == LOW) {
        delay(25); // this is for debounce

        if (digitalRead(pin) == LOW) {
            while (digitalRead(pin) ==LOW) {
                delay(5);
            }
            return true;
        }
    }
    return false;
}
void setup() {
    Serial.begin(115200);

    tft.init(76, 284); // screen size
    tft.setOffsets(82, 18); // offset to compermise for weird resolution
    tft.invertDisplay(false); // inverting colors
    tft.setRotation(1); // landscape, flip upsidedown is 3
    tft.fillScreen(ST77XX_BLACK); // clear screen

    wificonnection();

    Serial.println("TFT Initialied!");
    tft.setCursor(0,0); // cursor at top left
    tft.setTextSize(6);

    pinMode(SW_1, INPUT_PULLUP);
    pinMode(SW_2, INPUT_PULLUP);
    pinMode(SW_3, INPUT_PULLUP);
    pinMode(SW_4, INPUT_PULLUP);

    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);
}



void loop() {
    static unsigned long lastUpdate = 0;

    if (millis() - lastUpdate >= 1000) {
        lastUpdate = millis();
        drawtime();
    }
    if (buttonPressed(SW_1)) {
        buzzerbeep();
    }
    if (buttonPressed(SW_2)) {
        buzzerbeep();
    }
    if (buttonPressed(SW_3)) {
        clearscreen(); 
    }
    if (buttonPressed(SW_4)) {
        use24Hour = !use24Hour; //chance from 24 hour template to 12 hour template
        drawtime();
    }

}
