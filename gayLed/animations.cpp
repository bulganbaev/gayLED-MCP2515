#include <FS.h> // Include the SPIFFS library
#include "animations.h"
#include "utils.h"

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

extern volatile bool updatingSettings; // Flag to block loopAnimations

void initCustom();
void loadDefaultSettings();
void updateColorSetFromHex();
void loopAnimations();
void colorWave();
void breathingLight();
void rainbowCycle();
void canAnimations();
void rpmLevel();
void temperatureLevel();

void initCustom() {
    Serial.println("Initializing custom settings");
    loadDefaultSettings();
}

void loadDefaultSettings() {
    Serial.println("Loading default settings");
    // LGBT flag colors in hex
    custom_setting.colorSetHex[0] = 0xFF0000; // Red
    custom_setting.colorSetHex[1] = 0xFFA500; // Orange
    custom_setting.colorSetHex[2] = 0xFFFF00; // Yellow
    custom_setting.colorSetHex[3] = 0x008000; // Green
    custom_setting.colorSetHex[4] = 0x0000FF; // Blue
    custom_setting.colorSetHex[5] = 0x800080; // Purple

    custom_setting.colorCount = 6; // Six colors for the LGBT flag
    custom_setting.speedOfAnimation = 100;
    custom_setting.brightness = 255;
    custom_setting.animationIndex = 2;

    can_setting.maxTemp = 100;
    can_setting.minTemp = 50;
    can_setting.maxRPM = 7000;
    can_setting.minRPM = 1000;

    // Update colorSet from colorSetHex
    updateColorSetFromHex();
    Serial.println("Default settings loaded");
}

void updateColorSetFromHex() {
    for (int i = 0; i < custom_setting.colorCount; ++i) {
        uint32_t hexColor = custom_setting.colorSetHex[i];
        colorSet[i] = CRGB((hexColor >> 16) & 0xFF, (hexColor >> 8) & 0xFF, hexColor & 0xFF);
    }
}

void loopAnimations() {
    if (updatingSettings) {
      colorIndex = 0;
      return;
    }; // Block animations if settings are being updated

    // Serial.println("Looping animations");
    switch (custom_setting.animationIndex % 4) {
        case 0:
            // Serial.println("Running colorWave animation");
            colorWave();
            break;
        case 1:
            // Serial.println("Running breathingLight animation");
            breathingLight();
            break;
        case 2:
            // Serial.println("Running rainbowCycle animation");
            rainbowCycle();
            break;
        case 3:
            theaterChase();
            break;

    }
}
void theaterChase() {
    int delayTime = map(custom_setting.speedOfAnimation, 1, 100, 100, 1);
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    fill_solid(stop_leds, NUM_LEDS, CRGB::Black);

    int snakeLength = 20;
    for(int i=0; i<snakeLength; i++){
      stop_leds[i+wavePosition] = colorSet[0];
      leds[i+wavePosition] = colorSet[0];
    }
    wavePosition = wavePosition + 2*snakeLength/3;
    if(wavePosition > NUM_LEDS - snakeLength){
      wavePosition = wavePosition - NUM_LEDS + snakeLength;
    }
    FastLED.show();
    delay(delayTime);
}
void colorWave() {
    // Clear the strip before updating
    int delayTime = map(custom_setting.speedOfAnimation, 1, 100, 100, 1);
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    fill_solid(stop_leds, NUM_LEDS, CRGB::Black);

    // Calculate the number of LEDs per color section
    int ledsPerSection = NUM_LEDS / custom_setting.colorCount;

    // Iterate through each color section
    for (int i = 0; i < custom_setting.colorCount; i++) {
        for (int j = 0; j < ledsPerSection; j++) {
            if (i % 2 == 0) {
                // Set even sections to the corresponding color
                leds[i * ledsPerSection + j] = colorSet[i];
                stop_leds[i * ledsPerSection + j] = colorSet[i];


            } else {
                // Set odd sections to black
                leds[i * ledsPerSection + j] = CRGB::Black;
                stop_leds[i * ledsPerSection + j] = CRGB::Black;


            }
        }
    }
    FastLED.show();
    delay(delayTime);

    // Alternate the sections
    for (int i = 0; i < custom_setting.colorCount; i++) {
        for (int j = 0; j < ledsPerSection; j++) {
            if (i % 2 == 1) {
                // Set odd sections to the corresponding color
                leds[i * ledsPerSection + j] = colorSet[i];
                stop_leds[i * ledsPerSection + j] = colorSet[i];


            } else {
                // Set even sections to black
                leds[i * ledsPerSection + j] = CRGB::Black;
                stop_leds[i * ledsPerSection + j] = CRGB::Black;

            }
        }
    }
    FastLED.show();
    delay(delayTime);
}



void breathingLight() {
    int delayTime = map(custom_setting.speedOfAnimation, 1, 100, 10, 1);
    if (millis() - lastUpdateTime > (unsigned long)(delayTime)) {
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
            leds[i].fadeLightBy(255 - currentBrightness);
            stop_leds[i] = colorSet[colorIndex];
            stop_leds[i].fadeLightBy(255 - currentBrightness);
        }
        FastLED.show();
    }
}

void rainbowCycle() {
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CHSV((hue + i * 256 / NUM_LEDS) % 256, 255, custom_setting.brightness);
        stop_leds[i] = CHSV((hue + i * 256 / NUM_LEDS) % 256, 255, custom_setting.brightness);
    }
    FastLED.show();

    hue = (hue + map(custom_setting.speedOfAnimation, 1, 100, 1, 10)) % 256;

    // Map speedOfAnimation from 1-100 to delay range 200-10 milliseconds
    int delayTime = map(custom_setting.speedOfAnimation, 1, 100, 10, 1);
    delay(delayTime);
}

// Can Side
void canAnimations() {
    // Placeholder for CAN-specific animations
}

// Implementation of the chase animation
void rpmLevel() {
    int level = RPM; // Get the current RPM level

    fill_solid(leds, NUM_LEDS, CRGB::Black); // Turn off all LEDs initially
    fill_solid(stop_leds, NUM_LEDS, CRGB::Black);
    int MIN_LEVEL = can_setting.minRPM;
    int MAX_LEVEL = can_setting.maxRPM;

    // Calculate the number of LEDs to light up based on RPM level
    int numLedsToLight = map(level, MIN_LEVEL, MAX_LEVEL, 0, NUM_LEDS / 2);

    // Define transition points for color changes
    int firstTransitionPoint = numLedsToLight * 0.2; // Green to Orange transition
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
        stop_leds[indexStart] = color;
        stop_leds[indexEnd] = color;
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
                    stop_leds[indexStart] = CRGB::Red;
                    stop_leds[indexEnd] = CRGB::Red;
                }
            }
        }
    }
}

void temperatureLevel() {
    // Placeholder for temperature-related animation
}
