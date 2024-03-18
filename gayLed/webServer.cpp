//webServer.cpp
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

    server.on("/setMode", handleSetMode); 
    server.on("/obd", handleObd); // Handle OBD mode-specific page
    server.on("/custom", handleCustom);
    server.on("/obd-data", sendObdData);
    // Add more handlers as necessary

    server.begin();
    Serial.println("Web server started");
}

void handleRoot() {
    String htmlContent = FPSTR(HTML_HEADER);
    
    htmlContent += F("<div class='row heading'><h1>LED Controller</h1></div>");

    // Start of the dropdown menu in mode-selection row
    htmlContent += F("<div class='row mode-selection'><p>Select Mode:</p><br><form action='/setMode' method='get'><select id='mode' name='mode' class='dropdown' onchange='this.form.submit()'>");
    
    // Dynamically add options
    htmlContent += F("<option value='1'");
    htmlContent += (getCurrentMode() == MODE_ANIMATION_1 ? " selected" : "");
    htmlContent += F(">OBD2</option>");

    htmlContent += F("<option value='2'");
    htmlContent += (getCurrentMode() == MODE_ANIMATION_2 ? " selected" : "");
    htmlContent += F(">Custom</option>");
    
    // Close the dropdown and form
    htmlContent += F("</select></form></div>");

    htmlContent += "<div class='row mode-properties'><form id='modeForm' method='get' onsubmit='updateAction();'><button type='submit'>Go to Mode Properties</button></form></div>";

    // Append the script to dynamically update form action and the HTML footer
    htmlContent += FPSTR(HTML_SCRIPT);
    htmlContent += FPSTR(HTML_FOOTER);
    
    server.send(200, "text/html", htmlContent);
}




void handleSetMode() {
    if (server.hasArg("mode")) {
        int mode = server.arg("mode").toInt();
        switch(mode) {
            case 1:
                setMode(MODE_ANIMATION_1);
                break;
            case 2:
                setMode(MODE_ANIMATION_2);
                break;
            // Handle other modes as needed
        }
    }
    server.sendHeader("Location", "/", true); // Redirect back to the root page
    server.send(302, "text/plain", ""); // HTTP status code for temporary redirect
}
void handleObd() {
    String htmlContent = FPSTR(HTML_HEADER);

    htmlContent += R"(
        <h2>OBD Data</h2>
        <p>Coolant Temperature: <span id="coolantTemp">Loading...</span>°C</p>
        <p>Engine RPM: <span id="engineRpm">Loading...</span></p>
        <script>
            function fetchData() {
                fetch('/obd-data')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('coolantTemp').textContent = data.coolantTemp;
                    document.getElementById('engineRpm').textContent = data.engineRpm;
                })
                .catch(error => console.error('Error fetching data:', error));
            }
            setInterval(fetchData, 100);
            fetchData(); // Initial fetch
        </script>
    )";

    htmlContent += FPSTR(HTML_FOOTER);
    server.send(200, "text/html", htmlContent);
}


void handleCustom(){
  Serial.println("CUSTOM");
  server.sendHeader("Location", "/", true); // Redirect back to the root page
  server.send(302, "text/plain", "");
}

void sendObdData(){
  sendPID(PID_COOLANT_TEMP);
    delay(40); // Note: Using delay in web server callbacks isn't ideal
    int coolantTemp = receivePID(PID_COOLANT_TEMP);

    sendPID(PID_ENGINE_RPM);
    delay(40);
    int engineRpm = receivePID(PID_ENGINE_RPM);

    String json = "{\"coolantTemp\":" + String(coolantTemp) + ",\"engineRpm\":" + String(engineRpm) + "}";
    server.send(200, "application/json", json);
}
