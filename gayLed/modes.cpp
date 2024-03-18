#include <EEPROM.h>
#include "modes.h"

// Global variable to hold the current mode
volatile Mode currentMode = MODE_ANIMATION_1; // Default mode

void setMode(Mode mode) {
    if (currentMode != mode) {
        currentMode = mode;
        EEPROM.begin(512); // Initialize EEPROM
        EEPROM.put(MODE_EEPROM_ADDR, currentMode); // Save current mode
        EEPROM.commit(); // Ensure data is written to EEPROM
        EEPROM.end(); // Clean up
        Serial.println("Switched mode: " + String(currentMode));
    }
}

void modeHandler() {
    switch (currentMode) {
        case MODE_ANIMATION_1:
            rainbow();
            break;
        case MODE_ANIMATION_2:
            chase();
            break;
        // Extend with additional cases as needed
        default:
            // Optional: Handle unknown mode
            break;
    }
}

void initModeFromEEPROM() {
    EEPROM.begin(512); // Initialize EEPROM to read
    Mode savedMode;
    EEPROM.get(MODE_EEPROM_ADDR, savedMode); // Read mode from EEPROM
    EEPROM.end(); // Clean up

    // Validate the saved mode before using it
    if (savedMode >= MODE_ANIMATION_1 && savedMode <= MODE_ANIMATION_2) { // Adjust the range according to your actual modes
        currentMode = savedMode;
    } else {
        // If the saved mode is invalid, revert to a default mode
        currentMode = MODE_ANIMATION_1;
    }
    Serial.println("Current mode: " + String(currentMode));

}
Mode getCurrentMode() {
    return currentMode;
}