/*
  This is a simple sketch displaying some text and graphic on the LilyGo TTGO T-Display device.
  It comes with an assembled 1.14-inches TFT-display, driven by an ST7789 chip.

  The purpose is to show the transition of an Arduino to PlatformIO project, using two libraries:
  a) Adafruit ST7735 & ST7789 library by Adafruit (version 1.11.0, https://github.com/adafruit/Adafruit-ST7735-Library)
  b) InterruptButton by rwmingis (version 2.0.2)

  After pressing the Boot button the text "A Counter:" changes the color from skyblue to red and vice versa.

  Additionally, the Espressif ESP32 library and Arduino IDE version is shown.
*/

const char *PROGRAM_VERSION = "ESP32 LilyGo TTGO T-Display ST7789 1.14-inches display 135x240 px Adafruit ST7735 library V01";
const char *COUNTER_NAME = "A Counter:";

// ---------------------------------------------------------------------------------------------------
// Display library settings

#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ST7789.h>  // Hardware-specific library for ST7789
#include <SPI.h>

// GPIO settings for the display
#define TFT_MISO -1
#define TFT_MOSI 19
#define TFT_SCLK 18
#define TFT_CS 5
#define TFT_DC 16
#define TFT_RST 23
#define TFT_BL 4

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

#define COLOR_BLACK ST77XX_BLACK
#define COLOR_GREEN ST77XX_GREEN
#define COLOR_RED ST77XX_RED
#define COLOR_YELLOW ST77XX_YELLOW
#define COLOR_SKYBLUE 0x867D  // 135, 206, 235

// ---------------------------------------------------------------------------------------------------
// Button library settings
// https://github.com/rwmingis/InterruptButton
// WARNING: library InterruptButton claims to run on ESP32 architecture(s) and may be incompatible with your current board which runs on esp32 architecture(s).
#include "InterruptButton.h"  // version 2.0.2 (manual installation)
#define BOOT_BUTTON_PIN 0
InterruptButton button1(BOOT_BUTTON_PIN, LOW);  // Statically allocated button
bool isButtonSinglePressed = false;

void initButton() {
  // BIND BUTTON NUMBER 1 ACTIONS
  // Bind the address of a predefined function to an event as per below
  uint8_t thisMenuLevel = 0;
  button1.bind(Event_KeyPress, thisMenuLevel, []() {
    Serial.printf("Button 1: Key Up: %lu ms\n", millis());
    isButtonSinglePressed = true;
  });
}

// ---------------------------------------------------------------------------------------------------
// environment version management
#include "esp_idf_version.h"

// ---------------------------------------------------------------------------------------------------
// General variables

const uint8_t TEXT_SIZE = 2;
uint16_t counter = 0;
long lastUpdateCounter = 0;
const long UPDATE_COUNTER_DELAY_MILLIS = 1000;  // 1 second
bool toggleButtonAction = false;

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println(PROGRAM_VERSION);
  initButton();

  // Changing the Hardware SPI pins
  SPI.begin(TFT_SCLK, TFT_MISO, TFT_MOSI, TFT_CS);

  tft.init(135, 240);  // Init ST7789 240x135
  tft.setRotation(3);
  tft.setTextSize(TEXT_SIZE);
  tft.fillScreen(COLOR_BLACK);
  tft.setTextColor(COLOR_GREEN, COLOR_BLACK);
  tft.setCursor(20, 20);
  tft.print("LilyGo T-Display");
  tft.drawRect(0, 0, tft.width(), tft.height(), COLOR_RED);
  tft.setTextColor(COLOR_YELLOW, COLOR_BLACK);
  tft.setCursor(5, 95);
  tft.printf("IDF %d.%d.%d ", ESP_IDF_VERSION_MAJOR, ESP_IDF_VERSION_MINOR, ESP_IDF_VERSION_PATCH);
  tft.printf("AR %d.%d.%d", ESP_ARDUINO_VERSION_MAJOR, ESP_ARDUINO_VERSION_MINOR, ESP_ARDUINO_VERSION_PATCH);

  // set display brightness
  // required for Adafruit ST7735 library only
  analogWrite(TFT_BL, 255);  // full brightness
}

void displayCounter() {
  tft.setCursor(20, tft.height() / 2 - 10);
  if (toggleButtonAction) {
    tft.setTextColor(COLOR_RED, COLOR_BLACK);
  } else {
    tft.setTextColor(COLOR_SKYBLUE, COLOR_BLACK);
  }
  tft.print(COUNTER_NAME);
  tft.setTextColor(COLOR_SKYBLUE, COLOR_BLACK);
  tft.setCursor(150, tft.height() / 2 - 10);
  tft.print(String(counter));
}

void loop() {
  if (millis() - lastUpdateCounter > UPDATE_COUNTER_DELAY_MILLIS) {
    counter++;
    displayCounter();
    lastUpdateCounter = millis();
  }
  if (isButtonSinglePressed) {
    isButtonSinglePressed = false;
    toggleButtonAction = !toggleButtonAction;
    displayCounter();
  }
}
