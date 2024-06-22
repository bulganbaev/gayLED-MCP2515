//can_data.h
#ifndef CAN_DATA_H
#define CAN_DATA_H

#include <Arduino.h>

// Declaration of global variables
extern unsigned int RPM;
extern float oilTemperature;
extern bool brake;

// Declaration of the function to read CAN data and update global variables
void readCANDataAndUpdateGlobals();

// Declaration of the initialization function for CAN devices
void initializeCANDevices();

#endif // CAN_DATA_H
