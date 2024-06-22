//animations.h
#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include <FastLED.h> // Make sure to include FastLED here or in the main sketch
#include "config.h"
#include "can_data.h"


extern CRGB leds[];
extern CRGB stop_leds[];


#// Common variables
extern CRGB colorSet[];


struct CustomSettings {
  uint32_t colorSetHex[9]; // Store colors as hex values
  uint8_t colorCount; // Actual number of colors stored
  uint8_t speedOfAnimation;
  uint8_t brightness;
  uint8_t animationIndex;
};

struct CanSettings {
  uint8_t maxTemp; // Actual number of colors stored
  uint8_t minTemp;
  uint16_t minRPM;
  uint16_t maxRPM;

};

extern CustomSettings custom_setting;
extern CanSettings can_setting;


void initCustomFromEEPROM();
void loadDefaultSettings();
void saveSettings();

void loopAnimations();
void colorWave();
void breathingLight();
void rainbowCycle();

void canAnimations();
void rpmLevel();
void temperatureLevel();

CRGB convertToCRGB(String hexColor); 
uint32_t hexStringToUint32(String hex);
#endif // ANIMATIONS_H
