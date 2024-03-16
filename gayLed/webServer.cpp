#include "webServer.h"

// Define the web server on port 80
ESP8266WebServer server(80);

void startWebServer() {
    // Setup the access point
    WiFi.mode(WIFI_AP);
    WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
    Serial.println("Access Point Started");
    Serial.print("IP Address: ");
    Serial.println(WiFi.softAPIP());

    // Define URL handlers
    server.on("/", handleRoot);
    server.on("/mode1", handleMode1);
    server.on("/mode2", handleMode2);
    // Add more handlers as necessary

    server.begin();
    Serial.println("Web server started");
}

void handleRoot() {
    server.send(200, "text/html", MAIN_PAGE);
}

void handleMode1() {
    setMode(MODE_ANIMATION_1);
    server.send(200, "text/html", "<p>Mode 1 Activated</p><a href=\"/\">Back</a>");
}

void handleMode2() {
    setMode(MODE_ANIMATION_2);
    server.send(200, "text/html", "<p>Mode 2 Activated</p><a href=\"/\">Back</a>");
}
