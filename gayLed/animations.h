//animations.h
#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include <FastLED.h> // Make sure to include FastLED here or in the main sketch
#include "config.h"
extern CRGB leds[];
// Declare your animation functions here
void rainbow();
void chase();
void drawLevel();
#endif // ANIMATIONS_H
