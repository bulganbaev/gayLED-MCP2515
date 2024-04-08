#ifndef MODES_H
#define MODES_H

#include "config.h"
#include "animations.h"

// Define mode identifiers for easier handling
enum Mode {
    MODE_1 = 0,
    MODE_2 = 1,
    // Extend with additional modes as needed
};


// Function declarations
Mode getCurrentMode();
void setMode(Mode mode);
void modeHandler();
void initModeFromEEPROM();

#endif // MODES_H
