//config.h
#ifndef CONFIG_H
#define CONFIG_H

#define LED_PIN     D2
#define BRIGHTNESS  100
#define NUM_LEDS    300
#define LED_TYPE    WS2812B
#define COLOR_ORDER RGB
#define MAX_LEVEL   7000
#define MIN_LEVEL   1000
#define SIDE1       100
#define SIDE2       50

#define MODE_EEPROM_ADDR 0
#define ANIM_MODE_EEPROM_ADDR (MODE_EEPROM_ADDR + 1)

#define WIFI_SSID "ledControl"
#define WIFI_PASSWORD "pidor123" 

#endif // CONFIG_H