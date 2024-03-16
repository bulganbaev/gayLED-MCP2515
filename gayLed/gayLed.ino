//main.ino
#include <FastLED.h>


#include "config.h" 
#include "animations.h"
#include "webServer.h"
#include "modes.h"

CRGB leds[NUM_LEDS];


void setup() {
    Serial.begin(115200);
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
    initModeFromEEPROM();
    startWebServer();
}

void loop() {
    server.handleClient(); // Handle client requests
    modeHandler(); 
}
