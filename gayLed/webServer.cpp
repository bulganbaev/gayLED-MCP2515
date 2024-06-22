//webServer.cpp
#include "webServer.h"
#include "can_data.h"

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
  server.on("/obd", handleObd);  // Handle OBD mode-specific page
  server.on("/custom", handleCustom);
  server.on("/obd-data", sendObdData);
  server.on("/updateCustom", handleUpdateCustom);
  server.on("/updateCan", handleUpdateCan);
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
  htmlContent += (getCurrentMode() == MODE_1 ? " selected" : "");
  htmlContent += F(">OBD2</option>");

  htmlContent += F("<option value='2'");
  htmlContent += (getCurrentMode() == MODE_2 ? " selected" : "");
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
    switch (mode) {
      case 1:
        setMode(MODE_1);
        break;
      case 2:
        setMode(MODE_2);
        break;
        // Handle other modes as needed
    }
  }
  server.sendHeader("Location", "/", true);  // Redirect back to the root page
  server.send(302, "text/plain", "");        // HTTP status code for temporary redirect
}
void handleObd() {
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
            setInterval(fetchData, 100);
            fetchData(); // Initial fetch
        </script>   
  )";

  htmlContent += FPSTR(HTML_FOOTER);
  server.send(200, "text/html", htmlContent);
}


  void handleCustom() {
    String htmlContent = FPSTR(HTML_HEADER);
    htmlContent += R"(
      <h2>Update Animation Settings</h2>
      <form id="animationSettingsForm" action="/updateCustom" method="POST">
          <div id="colorPickers">
    )";

    // Dynamically add existing color pickers based on stored settings
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
              <input type="number" id="speedOfAnimation" name="speedOfAnimation" min="1" max="100" value=")" + String(1000 / custom_setting.speedOfAnimation) + R"(">
          </div>

          <div class="settings-container">
              <label for="brightness">Brightness:</label>
              <input type="number" id="brightness" name="brightness" min="0" max="255" value=")" + String(custom_setting.brightness) + R"(">
          </div>

          <div class="settings-container">
              <label for="animationIndex">Animation Type:</label>
              <select id="animationIndex" name="animationIndex">
    )";

    // Generating options and marking the current setting as selected
    const char* animationOptions[] = {"Color Wave", "Breathing Light", "Rainbow Cycle"};
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
                setRandomColor(newColorPicker.querySelector('input[type="color"]')); // Set a random color for this new color picker
                // Add event listener to the new "-" button
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

  int coolantTemp = oilTemperature;
  int engineRpm = RPM;

  String json = "{\"coolantTemp\":" + String(coolantTemp) + ",\"engineRpm\":" + String(engineRpm) + "}";
  server.send(200, "application/json", json);
}


void handleUpdateCustom() {
  custom_setting.colorCount = 0; // Reset color count
    // Extract and process each form field
    for (uint8_t i = 0; i < server.args(); i++) {
      if (server.argName(i).startsWith("colorSet")) {
        String colorValue = server.arg(i);
        if (custom_setting.colorCount < 9) { // Ensure within bounds
          custom_setting.colorSetHex[custom_setting.colorCount] = hexStringToUint32(colorValue);
          colorSet[custom_setting.colorCount] = convertToCRGB(colorValue); // Also update runtime CRGB array
          custom_setting.colorCount++;
        } else {
          break; // Avoid exceeding array bounds
        }
      }
    }

  // Parse other values
  custom_setting.speedOfAnimation = 1000 / server.arg("speedOfAnimation").toInt();
  custom_setting.brightness = server.arg("brightness").toInt();
  custom_setting.animationIndex = server.arg("animationIndex").toInt();

  // Save the updated settings to EEPROM
  saveSettings();

  // Apply settings to LEDs or other output (not shown)

  // Redirect or respond to the client after handling
  server.sendHeader("Location", "/", true); // Redirect back to the main page
  server.send(302, "text/plain", ""); // HTTP status code for redirection
}

void handleUpdateCan(){
  can_setting.minTemp = server.arg("minTemp").toInt();
  can_setting.maxTemp = server.arg("maxTemp").toInt();
  can_setting.minRPM = server.arg("minRPM").toInt();
  can_setting.maxRPM = server.arg("maxRPM").toInt();
  saveSettings();
  server.sendHeader("Location", "/", true); // Redirect back to the main page
  server.send(302, "text/plain", ""); // HTTP status code for redirection
}


