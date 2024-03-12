//main.ino
#include <FastLED.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include "config.h" 
#include "animations.h"
#include "stateManagement.h"

CRGB leds[NUM_LEDS];

extern OperatingMode currentMode;

void setup() {
    Serial.begin(115200);
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
    initializeMode();
    Serial.println("Current Mode:");
    Serial.print(currentMode);
}

void loop() {
  runCurrentAnimation();
}
