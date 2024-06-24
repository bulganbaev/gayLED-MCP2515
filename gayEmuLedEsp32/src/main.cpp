#include <Arduino.h>
#include "wifimanager_setup.h" // Подключаем наш заголовочный файл для WiFiManager

// Пин светодиода
#define LED_PIN 2

void setup() {
    // Инициализация последовательного порта
    Serial.begin(115200);
    Serial.println("Serial started");

    // Инициализация пина светодиода
    pinMode(LED_PIN, OUTPUT);
    Serial.println("LED pin initialized"+ String(LED_PIN));

    // Настройка WiFiManager
    Serial.println("Setting up WiFi");
    setupWiFi();
    Serial.println("WiFi setup complete");

    // Настройка OTA
    Serial.println("Setting up OTA");
    Serial.println("OTA setup complete");

}

void loop() {
    checkWiFiConnection();

    // Мигающий светодиод для проверки работы устройства
    digitalWrite(LED_PIN, HIGH);
    Serial.println("LED on");
    delay(1000);

    digitalWrite(LED_PIN, LOW);
    Serial.println("LED off");
    delay(1000);
}
