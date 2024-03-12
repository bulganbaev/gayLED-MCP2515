// StateManagement.h

#ifndef STATEMANAGEMENT_H
#define STATEMANAGEMENT_H

#include <EEPROM.h>

enum OperatingMode {
    MODE_OFF = 0,
    MODE_ANIMATION_1,
    MODE_ANIMATION_2,
    // Additional modes as necessary
    MODE_MAX
};

// Function declarations
void initializeMode();
void setMode(OperatingMode newMode);
void runCurrentAnimation();

// External declaration for current mode variable
extern OperatingMode currentMode;

#endif // STATEMANAGEMENT_H
