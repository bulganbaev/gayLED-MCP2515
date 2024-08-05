//can_data.cpp
#include "can_data.h"
#include "EMUcan.h"
#include <mcp2515.h>

// Initialization of global variables
unsigned int RPM = 0;
float oilTemperature = 0.0;
bool brake = false;

// Instantiate EMUcan and MCP2515 objects
EMUcan emucan(0x600); // Initialized with base ID 600
MCP2515 mcp2515(15); // MCP2515 with chip select (CS) pin 15

void initializeCANDevices() {
  mcp2515.reset();
  mcp2515.setBitrate(CAN_1000KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();
}

void readCANDataAndUpdateGlobals() {
    struct can_frame canMsg;
    if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
        emucan.checkEMUcan(canMsg.can_id, canMsg.can_dlc, canMsg.data);
        if (emucan.EMUcan_Status() == EMUcan_RECEIVED_WITHIN_LAST_SECOND) {
            RPM = emucan.emu_data.RPM;
            oilTemperature = emucan.emu_data.IAT;
        }
    }
    if (emucan.emu_data.flags1 & emucan.F_BRAKE_SWITCH) {
        brake = true;
    }
    else{
        brake = false;
    }


}
