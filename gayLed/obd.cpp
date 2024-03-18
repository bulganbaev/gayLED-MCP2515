// obd.cpp
#include "obd.h"

MCP_CAN CAN0(15); // Set CS to pin 15, adjust if needed

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
char msgString[128];  

bool obd_init() {
    
    // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s
    // and the masks and filters disabled.
    if (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
        // Serial.println("MCP2515 Initialized Successfully!");
        
        // Set operation mode to normal so the MCP2515 sends acks to received data.
        CAN0.setMode(MCP_NORMAL); 
        
        pinMode(CAN0_INT, INPUT); // Configuring pin for /INT input
        
        // Initialize mask and filter to only allow relevant CAN IDs
        CAN0.init_Mask(0, 0, 0x7FFFFFFF); // Use 0x7FFFFFFF for no mask
        CAN0.init_Filt(0, 0, CAN_ID_PID); // Set filter to OBD-II PID requests
        
        // Serial.println("OBD-II Initialization Complete.");
        return true; // Initialization successful
    } else {
        // Serial.println("Error Initializing MCP2515...");
        return false; // Initialization failed
    }
}

bool sendPID(unsigned char pid) {
    unsigned char message[8] = {0x02, 0x01, pid, 0, 0, 0, 0, 0};
    byte sendStatus = CAN0.sendMsgBuf(CAN_ID_PID, 0, 8, message);
    
    if (sendStatus == CAN_OK) {
        Serial.print("PID 0x");
        Serial.print(pid, HEX);
        Serial.println(" request sent successfully.");
        return true;
    } else {
        Serial.print("Error sending PID 0x");
        Serial.println(pid, HEX);
        return false;
    }
}

int receivePID(unsigned char pid) {
    int value = 0; // Initialize value to be returned

    if (!digitalRead(CAN0_INT)) { // If CAN0_INT pin is low, read receive buffer
        CAN0.readMsgBuf(&rxId, &len, rxBuf); // Read data
        
        // Check if the received ID and PID match what was requested
        if (rxId == CAN_ID_PID && rxBuf[2] == pid) {
            // Process data based on PID
            switch (pid) {
                case PID_COOLANT_TEMP:
                    // Convert to meaningful value
                    value = rxBuf[3] - 40;
                    break;
                case PID_ENGINE_RPM:
                    // Convert to meaningful value
                    value = ((unsigned int)rxBuf[3] * 256 + (unsigned int)rxBuf[4]) / 4;
                    break;
            }
        }
    }
    Serial.print("Received val:");
    Serial.println(value);
    return (int)value; // Return the processed value or 0 if not processed
}
