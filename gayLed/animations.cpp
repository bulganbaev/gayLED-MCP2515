#include <FS.h> // Include the SPIFFS library
#include "animations.h"
#include "utils.h"

CRGB colorSet[9] = {}; // here we need to be able to add from 1 to 10 colors
CustomSettings custom_setting;
CanSettings can_setting;

// Global state variables for animations
int wavePosition1 = 0;
int wavePosition2 = 0;
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
    loadCustomSettings();
    if (custom_setting.colorCount == 0 || custom_setting.colorCount > 9) {
        loadDefaultCustomSettings();
        saveCustomSettings();
    }
}
void initCan(){
    loadCanSettings();
    if (can_setting.minTemp == 0 || can_setting.minTemp > 200) {
        loadDefaultCanSettings();
        saveCanSettings();
    }
}

void loadDefaultCustomSettings() {
    Serial.println("Loading default settings");
    // LGBT flag colors in hex
    custom_setting.colorSetHex[0] = 0xff0000; // Red
    custom_setting.colorSetHex[1] = 0xffa500; // Orange
    custom_setting.colorSetHex[2] = 0xffff00; // Yellow
    custom_setting.colorSetHex[3] = 0x008000; // Green
    custom_setting.colorSetHex[4] = 0x0000ff; // Blue
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

void loadDefaultCanSettings() {
    Serial.println("Loading default settings");
    

    can_setting.maxTemp = 100;
    can_setting.minTemp = 50;
    can_setting.maxRPM = 7000;
    can_setting.minRPM = 1000;

    // Update colorSet from colorSetHex
    Serial.println("Default settings loaded");
}

void updateColorSetFromHex() {
    for (int i = 0; i < custom_setting.colorCount; ++i) {
        uint32_t hexColor = custom_setting.colorSetHex[i];
        colorSet[i] = CRGB((hexColor >> 16) & 0xff, (hexColor >> 8) & 0xff, hexColor & 0xff);
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

    // Clear both strips before updating
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    fill_solid(stop_leds, NUM_STOP_LEDS, CRGB::Black);

    int snakeLength = 20;

    // Update leds
    for (int i = 0; i < snakeLength; i++) {
        int ledIndex = (i + wavePosition1) % NUM_LEDS;
        leds[ledIndex] = colorSet[0];
    }

    // Update stop_leds
    for (int i = 0; i < snakeLength; i++) {
        int stopLedIndex = (i + wavePosition2) % NUM_STOP_LEDS;
        stop_leds[stopLedIndex] = colorSet[0];
    }

    wavePosition1 = (wavePosition1 + 2 * snakeLength / 3) % NUM_LEDS;
    wavePosition2 = (wavePosition2 + 2 * snakeLength / 3) % NUM_STOP_LEDS;
    FastLED.show();
    delay(delayTime);
}

void colorWave() {
    int delayTime = map(custom_setting.speedOfAnimation, 1, 100, 100, 1);

    // Clear both strips before updating
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    fill_solid(stop_leds, NUM_STOP_LEDS, CRGB::Black);

    // Calculate the number of LEDs per color section
    int ledsPerSection = NUM_LEDS / custom_setting.colorCount;
    int stopLedsPerSection = NUM_STOP_LEDS / custom_setting.colorCount;

    // Iterate through each color section for leds
    for (int i = 0; i < custom_setting.colorCount; i++) {
        for (int j = 0; j < ledsPerSection; j++) {
            int ledIndex = i * ledsPerSection + j;
            if (ledIndex < NUM_LEDS) {
                if (i % 2 == 0) {
                    // Set even sections to the corresponding color
                    leds[ledIndex] = colorSet[i];
                } else {
                    // Set odd sections to black
                    leds[ledIndex] = CRGB::Black;
                }
            }
        }
    }

    // Iterate through each color section for stop_leds
    for (int i = 0; i < custom_setting.colorCount; i++) {
        for (int j = 0; j < stopLedsPerSection; j++) {
            int stopLedIndex = i * stopLedsPerSection + j;
            if (stopLedIndex < NUM_STOP_LEDS) {
                if (i % 2 == 0) {
                    // Set even sections to the corresponding color
                    stop_leds[stopLedIndex] = colorSet[i];
                } else {
                    // Set odd sections to black
                    stop_leds[stopLedIndex] = CRGB::Black;
                }
            }
        }
    }

    FastLED.show();
    delay(delayTime);

    // Alternate the sections for leds
    for (int i = 0; i < custom_setting.colorCount; i++) {
        for (int j = 0; j < ledsPerSection; j++) {
            int ledIndex = i * ledsPerSection + j;
            if (ledIndex < NUM_LEDS) {
                if (i % 2 == 1) {
                    // Set odd sections to the corresponding color
                    leds[ledIndex] = colorSet[i];
                } else {
                    // Set even sections to black
                    leds[ledIndex] = CRGB::Black;
                }
            }
        }
    }

    // Alternate the sections for stop_leds
    for (int i = 0; i < custom_setting.colorCount; i++) {
        for (int j = 0; j < stopLedsPerSection; j++) {
            int stopLedIndex = i * stopLedsPerSection + j;
            if (stopLedIndex < NUM_STOP_LEDS) {
                if (i % 2 == 1) {
                    // Set odd sections to the corresponding color
                    stop_leds[stopLedIndex] = colorSet[i];
                } else {
                    // Set even sections to black
                    stop_leds[stopLedIndex] = CRGB::Black;
                }
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

        // Update leds
        for (int i = 0; i < NUM_LEDS; i++) {
            leds[i] = colorSet[colorIndex];
            leds[i].fadeLightBy(255 - currentBrightness);
        }

        // Update stop_leds
        for (int i = 0; i < NUM_STOP_LEDS; i++) {
            stop_leds[i] = colorSet[colorIndex];
            stop_leds[i].fadeLightBy(255 - currentBrightness);
        }

        FastLED.show();
    }
}

void rainbowCycle() {
    // Update leds
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CHSV((hue + i * 256 / NUM_LEDS) % 256, 255, custom_setting.brightness);
    }

    // Update stop_leds
    for (int i = 0; i < NUM_STOP_LEDS; i++) {
        stop_leds[i] = CHSV((hue + i * 256 / NUM_STOP_LEDS) % 256, 255, custom_setting.brightness);
    }

    FastLED.show();

    // Update hue
    hue = (hue + map(custom_setting.speedOfAnimation, 1, 100, 1, 10)) % 256;

    // Map speedOfAnimation from 1-100 to delay range 10-1 milliseconds
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
    fill_solid(stop_leds, NUM_STOP_LEDS, CRGB::Black);

    int MIN_LEVEL = can_setting.minRPM;
    int MAX_LEVEL = can_setting.maxRPM;

    // Calculate the number of LEDs to light up based on RPM level
    int numLedsToLight = map(level, MIN_LEVEL, MAX_LEVEL, 0, NUM_LEDS / 2);
    int numStopLedsToLight = map(level, MIN_LEVEL, MAX_LEVEL, 0, NUM_STOP_LEDS / 2);

    // Define transition points for color changes
    int firstTransitionPoint = NUM_LEDS / 2 * 0.1; // Green to Yellow transition
    int secondTransitionPoint = NUM_LEDS / 2 * 0.2; // Yellow to Orange transition
    int thirdTransitionPoint = NUM_LEDS / 2 * 0.3; // Orange to Red transition
    int fourthTransitionPoint = NUM_LEDS / 2 * 0.5; // Red to Purple transition
    int fifthTransitionPoint = NUM_LEDS / 2 * 0.7; // Purple to Pink transition
    int sixthTransitionPoint = NUM_LEDS / 2 * 0.9; // Pink to White transition

    for (int i = 0; i < numLedsToLight; i++) {
        // Calculate LED indices for both ends
        int indexStart = i;
        int indexEnd = (NUM_LEDS - 1) - i;

        // Initialize color blending factors and colors
        float blendFactor = 0;
        CRGB startColor;
        CRGB endColor;

        if (i <= firstTransitionPoint) {
            // Blue to Green transition
            blendFactor = float(i) / firstTransitionPoint;
            startColor = CRGB::Blue;
            endColor = CRGB::Green;
        } else if (i <= secondTransitionPoint) {
            // Green to Yellow transition
            blendFactor = float(i - firstTransitionPoint) / (secondTransitionPoint - firstTransitionPoint);
            startColor = CRGB::Green;
            endColor = CRGB::Yellow;
        } else if (i <= thirdTransitionPoint) {
            // Yellow to Orange transition
            blendFactor = float(i - secondTransitionPoint) / (thirdTransitionPoint - secondTransitionPoint);
            startColor = CRGB::Yellow;
            endColor = CRGB::Orange;
        } else if (i <= fourthTransitionPoint) {
            // Orange to Red transition
            blendFactor = float(i - thirdTransitionPoint) / (fourthTransitionPoint - thirdTransitionPoint);
            startColor = CRGB::Orange;
            endColor = CRGB::Red;
        } else if (i <= fifthTransitionPoint) {
            // Red to Purple transition
            blendFactor = float(i - fourthTransitionPoint) / (fifthTransitionPoint - fourthTransitionPoint);
            startColor = CRGB::Red;
            endColor = CRGB::Purple;
        } else if (i <= sixthTransitionPoint) {
            // Purple to Pink transition
            blendFactor = float(i - fifthTransitionPoint) / (sixthTransitionPoint - fifthTransitionPoint);
            startColor = CRGB::Purple;
            endColor = CRGB::Pink;
        } else {
            // Pink to White transition
            blendFactor = float(i - sixthTransitionPoint) / (numLedsToLight - sixthTransitionPoint);
            startColor = CRGB::Pink;
            endColor = CRGB::White;
        }

        // Blend the colors based on the calculated blend factor
        CRGB color = blend(startColor, endColor, uint8_t(blendFactor * 255));
        leds[indexStart] = color; // Set color for the starting LED
        leds[indexEnd] = color;   // Set color for the ending LED
    }

    // Define transition points for stop_leds color changes
    firstTransitionPoint = NUM_STOP_LEDS / 2 * 0.1; // Green to Yellow transition
    secondTransitionPoint = NUM_STOP_LEDS / 2 * 0.2; // Yellow to Orange transition
    thirdTransitionPoint = NUM_STOP_LEDS / 2 * 0.3; // Orange to Red transition
    fourthTransitionPoint = NUM_STOP_LEDS / 2 * 0.5; // Red to Purple transition
    fifthTransitionPoint = NUM_STOP_LEDS / 2 * 0.7; // Purple to Pink transition
    sixthTransitionPoint = NUM_STOP_LEDS / 2 * 0.9; // Pink to White transition

    for (int i = 0; i < numStopLedsToLight; i++) {
        // Calculate LED indices for both ends
        int indexStart = i;
        int indexEnd = (NUM_STOP_LEDS - 1) - i;

        // Initialize color blending factors and colors
        float blendFactor = 0;
        CRGB startColor;
        CRGB endColor;

        if (i <= firstTransitionPoint) {
            // Blue to Green transition
            blendFactor = float(i) / firstTransitionPoint;
            startColor = CRGB::Blue;
            endColor = CRGB::Green;
        } else if (i <= secondTransitionPoint) {
            // Green to Yellow transition
            blendFactor = float(i - firstTransitionPoint) / (secondTransitionPoint - firstTransitionPoint);
            startColor = CRGB::Green;
            endColor = CRGB::Yellow;
        } else if (i <= thirdTransitionPoint) {
            // Yellow to Orange transition
            blendFactor = float(i - secondTransitionPoint) / (thirdTransitionPoint - secondTransitionPoint);
            startColor = CRGB::Yellow;
            endColor = CRGB::Orange;
        } else if (i <= fourthTransitionPoint) {
            // Orange to Red transition
            blendFactor = float(i - thirdTransitionPoint) / (fourthTransitionPoint - thirdTransitionPoint);
            startColor = CRGB::Orange;
            endColor = CRGB::Red;
        } else if (i <= fifthTransitionPoint) {
            // Red to Purple transition
            blendFactor = float(i - fourthTransitionPoint) / (fifthTransitionPoint - fourthTransitionPoint);
            startColor = CRGB::Red;
            endColor = CRGB::Purple;
        } else if (i <= sixthTransitionPoint) {
            // Purple to Pink transition
            blendFactor = float(i - fifthTransitionPoint) / (sixthTransitionPoint - fifthTransitionPoint);
            startColor = CRGB::Purple;
            endColor = CRGB::Pink;
        } else {
            // Pink to White transition
            blendFactor = float(i - sixthTransitionPoint) / (numStopLedsToLight - sixthTransitionPoint);
            startColor = CRGB::Pink;
            endColor = CRGB::White;
        }

        // Blend the colors based on the calculated blend factor
        CRGB color = blend(startColor, endColor, uint8_t(blendFactor * 255));

        // Skip setting stop_leds in the range NUM_STOP_LEDS / 2 - 30 to NUM_STOP_LEDS / 2 + 30
        stop_leds[indexStart] = color;
        stop_leds[indexEnd] = color;
    }

    FastLED.show(); // Display the LED changes

 
}



void temperatureLevel() {
    // Placeholder for temperature-related animation
}


void saveCustomSettings() {
    EEPROM.begin(512); // Initialize EEPROM with size
    EEPROM.put(EEPROM_CUSTOM_SETTINGS_ADDR, custom_setting);
    EEPROM.commit(); // Ensure data is written to EEPROM
    Serial.println("Custom settings saved to EEPROM");
}

void loadCustomSettings() {
    EEPROM.begin(512); // Initialize EEPROM with size
    EEPROM.get(EEPROM_CUSTOM_SETTINGS_ADDR, custom_setting);
    Serial.println("Custom settings loaded from EEPROM");
    updateColorSetFromHex();
}

void saveCanSettings() {
    EEPROM.begin(512); // Initialize EEPROM with size
    EEPROM.put(EEPROM_CAN_SETTINGS_ADDR, can_setting);
    EEPROM.commit(); // Ensure data is written to EEPROM
    Serial.println("CAN settings saved to EEPROM");
}

void loadCanSettings() {
    EEPROM.begin(512); // Initialize EEPROM with size
    EEPROM.get(EEPROM_CAN_SETTINGS_ADDR, can_setting);
    Serial.println("CAN settings loaded from EEPROM");
}