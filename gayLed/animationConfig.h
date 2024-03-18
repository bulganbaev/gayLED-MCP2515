// AnimationConfig.h

#ifndef ANIMATION_CONFIG_H
#define ANIMATION_CONFIG_H

#include <FastLED.h> // For CRGB type

struct AnimationConfig {
  int animType;     // Animation type (e.g., anim1, anim2...)
  CRGB *colorSet;   // Pointer to an array of CRGB color values
  int colorCount;   // Number of colors in the set (for gradients)
  int speed;        // Speed (0-100)
};

#endif // ANIMATION_CONFIG_H
