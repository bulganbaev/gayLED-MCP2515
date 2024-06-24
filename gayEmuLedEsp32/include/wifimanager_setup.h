#ifndef WIFIMANAGER_SETUP_H
#define WIFIMANAGER_SETUP_H

#include <WiFi.h>
#include <WiFiManager.h>

// CSS для изменения внешнего вида страницы конфигурации
const char* customCSS = R"(
  body {
    background-color: #f7f7f7;
    color: #333;
    font-family: Arial, sans-serif;
  }
  .title {
    color: #ff6600;
  }
  .btn {
    background-color: #ff6600;
    color: white;
  }
  .btn:active {
    background-color: #ff4500;
  }
)";

void setupWiFi() {
    // Настройка WiFiManager
    WiFiManager wifiManager;

    // Убираем все вкладки
    wifiManager.setRemoveExtraTabs(true);

    // Добавление CSS для изменения внешнего вида
    wifiManager.setCustomHeadElement("<style>" + String(customCSS) + "</style>");

    // Имя и пароль для точки доступа
    const char* apName = "ZverCustomAPP";
    const char* apPassword = "robot123"; // Укажите ваш пароль

    // Создание новой точки доступа с паролем
    wifiManager.autoConnect(apName, apPassword); // Имя и пароль точки доступа

    // Проверка подключения
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Connected to WiFi");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("Failed to connect to WiFi");
    }
}
void checkWiFiConnection() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi disconnected, attempting to reconnect...");
        setupWiFi();
    }
}
#endif // WIFIMANAGER_SETUP_H
