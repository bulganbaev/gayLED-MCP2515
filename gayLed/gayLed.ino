//main.ino
#include <FastLED.h>


#include "config.h" 
#include "animations.h"
#include "webServer.h"
#include "modes.h"
#include "obd.h"

CRGB leds[NUM_LEDS];


void setup() {
    Serial.begin(115200); // Your program's baud rate
    delay(1000); // Wait a bit for the serial connection to stabilize
    Serial.println("Program starts now...");
    Serial.println("__________________________");

    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
    initModeFromEEPROM();
    startWebServer();
    if(obd_init() == false){
      Serial.println("Obd Error");
    }
}

void loop() {
    server.handleClient(); // Handle client requests
    modeHandler(); 
}
