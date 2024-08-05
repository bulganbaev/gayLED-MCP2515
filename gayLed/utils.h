//utils.h
#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <FastLED.h>

uint32_t hexStringToUint32(String hex);
uint32_t convertToCRGB(String hexValue);

#endif
