//modes.cpp
#include <FS.h> // Include the SPIFFS library
#include "modes.h"

// Global variable to hold the current mode
volatile Mode currentMode = MODE_1; // Default mode

void setMode(Mode mode) {
    if (currentMode != mode) {
        Serial.println("Switched to: " + String(mode));
        currentMode = mode;
    }
}

void modeHandler() {
    if(RPM < can_setting.minRPM){
      setMode(MODE_2);
    }
    else{
      setMode(MODE_1);
    }
    switch (currentMode) {
        case MODE_1:
            digitalWrite(LED_BUILTIN, LOW);
            rpmLevel();
            break;
        case MODE_2:
            digitalWrite(LED_BUILTIN, HIGH);
            loopAnimations();
            break;
        // Extend with additional cases as needed
        default:
            // Optional: Handle unknown mode
            break;
    }
}

void initMode() {
    currentMode = MODE_1;
}

Mode getCurrentMode() {
    return currentMode;
}
