//main.ino
#include <FastLED.h>


#include "config.h" 
#include "animations.h"
#include "webServer.h"
#include "modes.h"
#include "can_data.h"

CRGB leds[NUM_LEDS];
CRGB stop_leds[NUM_STOP_LEDS];


void setup() {
    Serial.begin(115200); // Your program's baud rate
    delay(1000); // Wait a bit for the serial connection to stabilize
    Serial.println("Program starts now...");
    Serial.println("__________________________");

    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.addLeds<LED_TYPE, LED_STOP_PIN, COLOR_ORDER>(stop_leds, NUM_STOP_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
    initModeFromEEPROM();
    initCustomFromEEPROM();
    startWebServer();
    initializeCANDevices();
    pinMode(LED_BUILTIN, OUTPUT);

}

void loop() {
    server.handleClient(); 
    modeHandler(); 
  

}
