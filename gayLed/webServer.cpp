#include "webServer.h"
#include "can_data.h"
#include "utils.h"  // Include the new header file

// Define the web server on port 80
ESP8266WebServer server(80);
volatile bool updatingSettings = false; // Flag to block loopAnimations while updating settings

void startWebServer();
void handleRoot();
void handleSetMode();
void handleObd();
void handleCustom();
void sendObdData();
void handleUpdateCustom();
void handleUpdateCan();
bool customSettingsChanged();
bool canSettingsChanged();
int countColorArgs();

void startWebServer() {
    Serial.println("Starting Web Server...");

    // Setup the access point
    WiFi.mode(WIFI_AP);
    WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
    Serial.println("Access Point Started");
    Serial.print("IP Address: ");
    Serial.println(WiFi.softAPIP());

    // Define URL handlers
    server.on("/", handleRoot);
    server.on("/setMode", handleSetMode);
    server.on("/obd", handleObd);  // Handle OBD mode-specific page
    server.on("/custom", handleCustom);
    server.on("/obd-data", sendObdData);
    server.on("/updateCustom", handleUpdateCustom);
    server.on("/updateCan", handleUpdateCan);

    server.begin();
    Serial.println("Web server started");
}

void handleRoot() {
    Serial.println("Handling Root Request");

    String htmlContent = FPSTR(HTML_HEADER);
    htmlContent += F("<div class='row heading'><h1>LED Controller</h1></div>");
    htmlContent += F("<div class='row mode-selection'><p>Select Mode:</p><br><form action='/setMode' method='get'><select id='mode' name='mode' class='dropdown' onchange='this.form.submit()'>");

    htmlContent += F("<option value='1'");
    htmlContent += (getCurrentMode() == MODE_1 ? " selected" : "");
    htmlContent += F(">OBD2</option>");

    htmlContent += F("<option value='2'");
    htmlContent += (getCurrentMode() == MODE_2 ? " selected" : "");
    htmlContent += F(">Custom</option>");

    htmlContent += F("</select></form></div>");
    htmlContent += "<div class='row mode-properties'><form id='modeForm' method='get' onsubmit='updateAction();'><button type='submit'>Go to Mode Properties</button></form></div>";
    htmlContent += FPSTR(HTML_SCRIPT);
    htmlContent += FPSTR(HTML_FOOTER);

    server.send(200, "text/html", htmlContent);
}

void handleSetMode() {
    Serial.println("Handling Set Mode Request");

    if (server.hasArg("mode")) {
        int mode = server.arg("mode").toInt();
        Serial.print("Selected mode: ");
        Serial.println(mode);

        switch (mode) {
            case 1:
                setMode(MODE_1);
                Serial.println("Mode set to OBD2");
                break;
            case 2:
                setMode(MODE_2);
                Serial.println("Mode set to Custom");
                break;
            default:
                Serial.println("Invalid mode selected");
                break;
        }
    }
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
}

void handleObd() {
    Serial.println("Handling OBD Request");

    String htmlContent = FPSTR(HTML_HEADER);
    htmlContent += R"(
        <h2>OBD Data</h2>
        <p>Coolant Temperature: <span id="coolantTemp">Loading...</span>°C</p>
        <p>Engine RPM: <span id="engineRpm">Loading...</span></p>
        <form id="animationSettingsForm" action="/updateCan" method="POST">
            <div class="settings-container">
                <label for="minTemp">Minimal Temperature</label>
                <input type="number" id="minTemp" name="minTemp" min="1" max="200" value=")" + String(can_setting.minTemp) + R"(">
            </div>
            <div class="settings-container">
                <label for="maxTemp">Maximum Temperature</label>
                <input type="number" id="maxTemp" name="maxTemp" min="1" max="200" value=")" + String(can_setting.maxTemp) + R"(">
            </div>
            <div class="settings-container">
                <label for="minRPM">Minimal RPM</label>
                <input type="number" id="minRPM" name="minRPM" min="1" max="9000" value=")" + String(can_setting.minRPM) + R"(">
            </div>
            <div class="settings-container">
                <label for="maxRPM">Maximum RPM</label>
                <input type="number" id="maxRPM" name="maxRPM" min="1" max="9000" value=")" + String(can_setting.maxRPM) + R"(">
            </div>
            <input type="submit" value="Update Settings">
        </form>
    )";
    htmlContent += R"(
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
            setInterval(fetchData, 1000);
            fetchData();
        </script>
    )";
    htmlContent += FPSTR(HTML_FOOTER);
    server.send(200, "text/html", htmlContent);
}

void handleCustom() {
    Serial.println("Handling Custom Request");

    String htmlContent = FPSTR(HTML_HEADER);
    htmlContent += R"(
        <h2>Update Animation Settings</h2>
        <form id="animationSettingsForm" action="/updateCustom" method="POST">
            <div id="colorPickers">
    )";
    for (uint8_t i = 0; i < custom_setting.colorCount; i++) {
        char hexColor[8];
        sprintf(hexColor, "#%06X", custom_setting.colorSetHex[i]);
        htmlContent += "<div class=\"color-picker-container\"><input type=\"color\" name=\"colorSet[]\" value=\"" + String(hexColor) + "\"><button type=\"button\" class=\"removeColor\">-</button></div>";
    }
    htmlContent += R"(
            </div>
            <button type="button" id="addColor">+</button><br><br>
            <div class="settings-container">
                <label for="speedOfAnimation">Speed of Animation:</label>
                <input type="number" id="speedOfAnimation" name="speedOfAnimation" min="1" max="100" value=")" + String(custom_setting.speedOfAnimation) + R"(">
            </div>
            <div class="settings-container">
                <label for="brightness">Brightness:</label>
                <input type="number" id="brightness" name="brightness" min="0" max="255" value=")" + String(custom_setting.brightness) + R"(">
            </div>
            <div class="settings-container">
                <label for="animationIndex">Animation Type:</label>
                <select id="animationIndex" name="animationIndex">
    )";
    const char* animationOptions[] = {"Color Wave", "Breathing Light", "Rainbow Cycle", "Theater Chase"};
    for (uint8_t i = 0; i < sizeof(animationOptions) / sizeof(animationOptions[0]); i++) {
        htmlContent += "<option value=\"" + String(i) + "\"";
        if (i == custom_setting.animationIndex) {
            htmlContent += " selected";
        }
        htmlContent += ">" + String(animationOptions[i]) + "</option>";
    }
    htmlContent += R"(
                </select><br>
            </div>
            <input type="submit" value="Update Settings">
        </form>
        <script>
            document.addEventListener('DOMContentLoaded', function() {
                document.getElementById('addColor').addEventListener('click', function() {
                    var container = document.getElementById('colorPickers');
                    if(container.children.length < 9) { // Allows up to 10 color pickers including the original
                        var newColorPicker = document.createElement('div');
                        newColorPicker.classList.add('color-picker-container');
                        newColorPicker.innerHTML = '<input type="color" name="colorSet[]" value="#ffffff"><button type="button" class="removeColor">-</button>';
                        container.appendChild(newColorPicker);
                        setRandomColor(newColorPicker.querySelector('input[type="color"]'));
                        newColorPicker.querySelector('.removeColor').addEventListener('click', function() {
                            this.parentNode.remove();
                        });
                    } else {
                        alert('You can add up to 9 colors.');
                    }
                });
                document.getElementById('colorPickers').addEventListener('click', function(e) {
                    if(e.target && e.target.className === 'removeColor') {
                        e.target.parentNode.remove();
                    }
                });
                function setRandomColor(colorInput) {
                    var randomColor = '#' + Math.floor(Math.random()*16777215).toString(16).padStart(6, '0');
                    colorInput.value = randomColor;
                }
            });
        </script>
    )";
    htmlContent += FPSTR(HTML_FOOTER);
    server.send(200, "text/html", htmlContent);
}

void sendObdData() {
    Serial.println("Sending OBD Data");

    int coolantTemp = oilTemperature;
    int engineRpm = RPM;
    String json = "{\"coolantTemp\":" + String(coolantTemp) + ",\"engineRpm\":" + String(engineRpm) + "}";
    server.send(200, "application/json", json);
}

bool customSettingsChanged() {
    if (custom_setting.speedOfAnimation != 5000 / server.arg("speedOfAnimation").toInt() ||
        custom_setting.brightness != server.arg("brightness").toInt() ||
        custom_setting.animationIndex != server.arg("animationIndex").toInt() ||
        custom_setting.colorCount != countColorArgs()) {
        return true;
    }

    // Compare each color value as CRGB objects
    for (uint8_t i = 0; i < custom_setting.colorCount; i++) {
        CRGB currentColor = CRGB(custom_setting.colorSetHex[i]);
        CRGB newColor = convertToCRGB(server.arg("colorSet[" + String(i) + "]"));
        if (currentColor != newColor) {
            return true;
        }
    }

    return false;
}

bool canSettingsChanged() {
    return (can_setting.minTemp != server.arg("minTemp").toInt() ||
            can_setting.maxTemp != server.arg("maxTemp").toInt() ||
            can_setting.minRPM != server.arg("minRPM").toInt() ||
            can_setting.maxRPM != server.arg("maxRPM").toInt());
}

int countColorArgs() {
    int count = 0;
    for (uint8_t i = 0; i < server.args(); i++) {
        if (server.argName(i).startsWith("colorSet")) {
            count++;
        }
    }
    return count;
}

void handleUpdateCustom() {
    Serial.println("Handling Update Custom Request");

    if (!customSettingsChanged()) {
        Serial.println("No changes in custom settings detected.");
        server.sendHeader("Location", "/", true);
        server.send(302, "text/plain", "");
        return;
    }

    updatingSettings = true; // Block loopAnimations

    custom_setting.colorCount = 0;
    for (uint8_t i = 0; i < server.args(); i++) {
        if (server.argName(i).startsWith("colorSet")) {
            String color = server.arg(i);
            Serial.print("Color Value: ");
            Serial.println(color);
            if (custom_setting.colorCount < 9) {
                uint32_t hexcolor = convertToCRGB(color);
                custom_setting.colorSetHex[custom_setting.colorCount] = hexcolor;
                colorSet[custom_setting.colorCount] = CRGB((hexcolor >> 16) & 0xff, (hexcolor >> 8) & 0xff, hexcolor & 0xff);
                custom_setting.colorCount++;
            } else {

                
            }
        }
    }

    custom_setting.speedOfAnimation = server.arg("speedOfAnimation").toInt();
    custom_setting.brightness = server.arg("brightness").toInt();
    custom_setting.animationIndex = server.arg("animationIndex").toInt();

    Serial.println("Custom settings updated");
    updatingSettings = false; // Unblock loopAnimations
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
}

void handleUpdateCan() {
    Serial.println("Handling Update CAN Request");

    if (!canSettingsChanged()) {
        Serial.println("No changes in CAN settings detected.");
        server.sendHeader("Location", "/", true);
        server.send(302, "text/plain", "");
        return;
    }

    updatingSettings = true; // Block loopAnimations

    can_setting.minTemp = server.arg("minTemp").toInt();
    can_setting.maxTemp = server.arg("maxTemp").toInt();
    can_setting.minRPM = server.arg("minRPM").toInt();
    can_setting.maxRPM = server.arg("maxRPM").toInt();

    Serial.println("CAN settings updated");
    updatingSettings = false; // Unblock loopAnimations
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
}
