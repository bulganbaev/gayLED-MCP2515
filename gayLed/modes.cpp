//modes.cpp
#include <FS.h> // Include the SPIFFS library
#include "modes.h"

// Global variable to hold the current mode
volatile Mode currentMode = MODE_1; // Default mode

void setMode(Mode mode) {
    if (currentMode != mode) {
        currentMode = mode;
        
        // Initialize SPIFFS
        if (!SPIFFS.begin()) {
            Serial.println("SPIFFS initialization failed!");
            return;
        }

        // Save current mode to SPIFFS
        File writeFile = SPIFFS.open("/mode.txt", "w");
        if (!writeFile) {
            Serial.println("Failed to open file for writing");
            return;
        }
        writeFile.println(currentMode);
        writeFile.close();
        SPIFFS.end(); // Clean up
        Serial.println("Switched mode: " + String(currentMode));
    }
}

void modeHandler() {
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

void initModeFromEEPROM() {
    // Initialize SPIFFS
    if (!SPIFFS.begin()) {
        Serial.println("SPIFFS initialization failed!");
        return;
    }

    // Read mode from SPIFFS
    File readFile = SPIFFS.open("/mode.txt", "r");
    if (!readFile) {
        Serial.println("Failed to open file for reading");
        return;
    }

    String modeString = readFile.readStringUntil('\n');
    int savedMode = modeString.toInt();
    
    // Validate the saved mode before using it
    if (savedMode >= MODE_1 && savedMode <= MODE_2) { // Adjust the range according to your actual modes
        currentMode = static_cast<Mode>(savedMode);
    } else {
        // If the saved mode is invalid, revert to a default mode
        currentMode = MODE_1;
    }

    readFile.close();
    SPIFFS.end(); // Clean up
    Serial.println("Current mode: " + String(currentMode));
}

Mode getCurrentMode() {
    return currentMode;
}
