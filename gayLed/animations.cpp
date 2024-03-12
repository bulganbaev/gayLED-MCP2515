//animations.cpp
#include "animations.h"

// Implementation of the rainbow animation
void rainbow() {
    // A simple rainbow animation
    static uint8_t hue = 0;
    for(int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CHSV(hue++, 255, 255);
    }
    FastLED.show();
}

// Implementation of the chase animation
void chase() {
    static int leadDot = 0;
    fill_solid(leds, NUM_LEDS, CRGB::Black); // Turn all LEDs off
    leds[leadDot] = CRGB::White; // Turn on the lead dot
    if(++leadDot >= NUM_LEDS) leadDot = 0;
    FastLED.show();
}

// Implementation of the chase animation
void drawLevel() {
    int level = random(7001);
    fill_solid(leds, NUM_LEDS, CRGB::Black);

    int centerLed = (2 * SIDE1 + SIDE2) + (SIDE2 / 2) - 1; 
    int numLedsToLight = map(level, MIN_LEVEL, MAX_LEVEL, 0, SIDE1 + SIDE2);

    int firstTransitionPoint = numLedsToLight * 0.3; // Transition for green to orange
    int secondTransitionPoint = numLedsToLight * 0.5; // Transition for orange to red

    for (int i = 0; i < numLedsToLight; i++) {
        int indexLeft = (centerLed - i + NUM_LEDS) % NUM_LEDS;
        int indexRight = (centerLed + i) % NUM_LEDS;

        // Calculate blend factor
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

        CRGB color = blend(startColor, endColor, blendFactor * 255);
        leds[indexLeft] = color;
        leds[indexRight] = color;
    }

    FastLED.show();

    // Blinking effect for the last 20% if level is 7000, adjusting the blink delay
    if (level == MAX_LEVEL) {
        static bool blinkState = false;
        static unsigned long lastBlinkTime = 0;
        unsigned long currentMillis = millis();
        if (currentMillis - lastBlinkTime > 500) { // Blink every 500 milliseconds
            blinkState = !blinkState;
            lastBlinkTime = currentMillis;

            if (!blinkState) {
                for (int i = secondTransitionPoint; i < numLedsToLight; i++) {
                    int indexLeft = (centerLed - i + NUM_LEDS) % NUM_LEDS;
                    int indexRight = (centerLed + i) % NUM_LEDS;
                    leds[indexLeft] = CRGB::Red; // Reapply red for blinking off state
                    leds[indexRight] = CRGB::Red;
                }
            }
        }
    }
}

