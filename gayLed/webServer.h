#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "htmlPages.h"
#include "animations.h"
#include "config.h"
#include "modes.h" // Include modes for setting animation modes

extern ESP8266WebServer server; // Declare server as an external variable to be defined in webServer.cpp

// Function declarations for initializing the server and handling requests
void startWebServer();
void handleRoot();
void handleMode1();
void handleMode2();
void handleSetMode();
void handleObd();
void handleCustom();
void handleUpdateCustom();
void handleUpdateCan();
void sendObdData();

#endif // WEBSERVER_H
