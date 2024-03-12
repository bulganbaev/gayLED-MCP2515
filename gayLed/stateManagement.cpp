// StateManagement.cpp

#include "StateManagement.h"
#include "config.h"

OperatingMode currentMode = MODE_ANIMATION_1; // Default mode

void EEPROM_WriteIfNeeded(int address, uint8_t value) {
    uint8_t currentValue = EEPROM.read(address);
    if (currentValue != value) {
        EEPROM.write(address, value);
    }
}

void initializeMode() {
    uint8_t savedMode = EEPROM.read(MODE_EEPROM_ADDR);
    if (savedMode >= MODE_OFF && savedMode < MODE_MAX) {
        currentMode = static_cast<OperatingMode>(savedMode);
    } else {
        currentMode = MODE_OFF; // Default to OFF mode if the stored value is invalid
        EEPROM_WriteIfNeeded(MODE_EEPROM_ADDR, currentMode); // Correct the EEPROM if necessary
    }
}

void setMode(OperatingMode newMode) {
    if (currentMode != newMode && newMode < MODE_MAX) {
        currentMode = newMode;
        EEPROM_WriteIfNeeded(MODE_EEPROM_ADDR, currentMode);
    }
}

void runCurrentAnimation() {
    switch (currentMode) {
        case MODE_ANIMATION_1:
            break;
        case MODE_ANIMATION_2:
            // Your animation 2 code
            break;
        // Implement additional cases as needed
        default:
            // MODE_OFF or any unrecognized mode
            break;
    }
}
