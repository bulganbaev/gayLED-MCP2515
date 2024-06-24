#include "utils.h"

uint32_t hexStringToUint32(String hex) {
    // Remove '#' if present
    if (hex.startsWith("#")) {
        hex = hex.substring(1);
    }
    // Ensure the hex string is 6 characters long
    while (hex.length() < 6) {
        hex = "0" + hex;
    }
    // Convert and return the hexadecimal string as a uint32_t value
    uint32_t result = (uint32_t) strtoul(hex.c_str(), nullptr, 16);
    Serial.print("Converted hex string to uint32_t: ");
    Serial.println(result, HEX);
    return result;
}

uint32_t convertToCRGB(String hexValue) {
    Serial.print("Converting hex value to CRGB: ");
    Serial.println(hexValue);

    uint32_t num = hexStringToUint32(hexValue);
    return (num >> 16 & 0xFF) << 16 | (num >> 8 & 0xFF) << 8 | (num & 0xFF);
}
