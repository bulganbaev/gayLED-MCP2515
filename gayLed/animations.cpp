//animations.cpp
#include <FS.h> // Include the SPIFFS library
#include "animations.h"


CRGB colorSet[9] = {}; // here we need to be able to add from 1 to 10 colors
CustomSettings custom_setting;
CanSettings can_setting;

// Global state variables for animations
int wavePosition = 0;
int colorIndex = 0;
int currentBrightness = 0;
bool gettingBrighter = true;
unsigned long lastUpdateTime = 0;
int hue = 0;

void initCustom() {
    loadDefaultSettings()
}

void loadDefaultSettings() {
    // LGBT flag colors in hex
    custom_setting.colorSetHex[0] = 0xFF0000; // Red
    custom_setting.colorSetHex[1] = 0xFFA500; // Orange
    custom_setting.colorSetHex[2] = 0xFFFF00; // Yellow
    custom_setting.colorSetHex[3] = 0x008000; // Green
    custom_setting.colorSetHex[4] = 0x0000FF; // Blue
    custom_setting.colorSetHex[5] = 0x800080; // Purple

    custom_setting.colorCount = 6; // Six colors for the LGBT flag
    custom_setting.speedOfAnimation = 10;
    custom_setting.brightness = 255;
    custom_setting.animationIndex = 0;

    can_setting.maxTemp = 100;
    can_setting.minTemp = 50;
    can_setting.maxRPM = 7000;
    can_setting.minRPM = 1000;

    // Convert hex colors to CRGB for runtime use
    for (int i = 0; i < custom_setting.colorCount; ++i) {
        uint32_t hexColor = custom_setting.colorSetHex[i];
        colorSet[i] = CRGB((hexColor >> 16) & 0xFF, (hexColor >> 8) & 0xFF, hexColor & 0xFF);
    }
}


void loopAnimations() {
    switch(custom_setting.animationIndex % 3) {
        case 0:
            colorWave();
            break;
        case 1:
            breathingLight();
            break;
        case 2:
            rainbowCycle();
            break;
    }
}

void colorWave() {
    fill_solid(leds, NUM_LEDS, colorSet[colorIndex]); // Set the whole strip to the current color
    FastLED.show();
    wavePosition++;
    if(wavePosition >= NUM_LEDS) {
        wavePosition = 0;
        colorIndex = (colorIndex + 1) % custom_setting.colorCount;
    }
    delay(100 - custom_setting.speedOfAnimation);
}

void breathingLight() {
    if (millis() - lastUpdateTime > (unsigned long)(100 - custom_setting.speedOfAnimation)) {
        lastUpdateTime = millis();
        if (gettingBrighter) {
            currentBrightness += 5;
            if (currentBrightness >= custom_setting.brightness) {
                currentBrightness = custom_setting.brightness;
                gettingBrighter = false;
            }
        } else {
            currentBrightness -= 5;
            if (currentBrightness <= 0) {
                currentBrightness = 0;
                gettingBrighter = true;
                colorIndex = (colorIndex + 1) % custom_setting.colorCount;
            }
        }

        for (int i = 0; i < NUM_LEDS; i++) {
            leds[i] = colorSet[colorIndex];
            leds[i].fadeToBlackBy(255 - currentBrightness);
        }
        FastLED.show();
    }
}

void rainbowCycle() {
    for(int i = 0; i < NUM_LEDS; i++) {
        int colorIndex = (hue + i * (256 / custom_setting.colorCount)) % 256;
        leds[i] = CHSV(colorIndex, 255, 255);
    }
    FastLED.show();
    hue = (hue + custom_setting.speedOfAnimation) % 256;
    delay(20);
}

// Can Side
void canAnimations(){

}

// Implementation of the chase animation
void rpmLevel() {
    int level = RPM; // Get the current RPM level
    fill_solid(leds, NUM_LEDS, CRGB::Black); // Turn off all LEDs initially

    // Calculate the number of LEDs to light up based on RPM level
    int numLedsToLight = map(level, MIN_LEVEL, MAX_LEVEL, 0, NUM_LEDS / 2);

    // Define transition points for color changes
    int firstTransitionPoint = numLedsToLight * 0.3; // Green to Orange transition
    int secondTransitionPoint = numLedsToLight * 0.5; // Orange to Red transition

    for (int i = 0; i < numLedsToLight; i++) {
        // Calculate LED indices for both ends
        int indexStart = i;
        int indexEnd = (NUM_LEDS - 1) - i;

        // Initialize color blending factors and colors
        float blendFactor = 0;
        CRGB startColor;
        CRGB endColor;

        if (i <= firstTransitionPoint) {
            // Green to Orange transition
            blendFactor = float(i) / firstTransitionPoint;
            startColor = CRGB::Green;
            endColor = CRGB::Orange;
        } else if (i <= secondTransitionPoint) {
            // Orange to Red transition
            blendFactor = float(i - firstTransitionPoint) / (secondTransitionPoint - firstTransitionPoint);
            startColor = CRGB::Orange;
            endColor = CRGB::Red;
        } else {
            // Solid Red for the last part
            startColor = endColor = CRGB::Red;
        }

        // Blend the colors based on the calculated blend factor
        CRGB color = blend(startColor, endColor, blendFactor * 255);
        leds[indexStart] = color; // Set color for the starting LED
        leds[indexEnd] = color; // Set color for the ending LED
    }

    FastLED.show(); // Display the LED changes

    // Blinking effect for the last 20% if RPM level is at MAX_LEVEL
    if (level == MAX_LEVEL) {
        static bool blinkState = false;
        static unsigned long lastBlinkTime = 0;
        unsigned long currentMillis = millis();
        if (currentMillis - lastBlinkTime > 500) { // Blink every 500 milliseconds
            blinkState = !blinkState;
            lastBlinkTime = currentMillis;

            if (!blinkState) {
                // Turn off LEDs for the blinking effect
                for (int i = secondTransitionPoint; i < numLedsToLight; i++) {
                    int indexStart = i;
                    int indexEnd = (NUM_LEDS - 1) - i;
                    leds[indexStart] = CRGB::Red; // Reapply red for blinking off state
                    leds[indexEnd] = CRGB::Red;
                }
            }
        }
    }
}


void temperatureLevel(){
    // int currentTemp = getTempValue();
}

CRGB convertToCRGB(String hexValue) {
    uint32_t num = hexStringToUint32(hexValue);
    return CRGB((num >> 16) & 0xFF, (num >> 8) & 0xFF, num & 0xFF);
}

uint32_t hexStringToUint32(String hex) {
    // Assuming hex is prefixed with "#", remove it
    if (hex.startsWith("#")) {
        hex = hex.substring(1);
    }
    // Convert and return the hexadecimal string as a uint32_t value
    return (uint32_t) strtoul(hex.c_str(), nullptr, 16);
}
