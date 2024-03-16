#ifndef HTMLPAGES_H
#define HTMLPAGES_H

#include <pgmspace.h> // Include for PROGMEM functionality

// Define a constant char array to store your HTML page in program memory
const char MAIN_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>LED Controller</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 0; padding: 20px; background-color: #f0f0f0; }
        h1 { color: #333; }
        .button { display: inline-block; background-color: #008CBA; color: white; padding: 15px 25px; text-align: center; text-decoration: none; font-size: 16px; margin: 4px 2px; cursor: pointer; }
    </style>
</head>
<body>
    <h1>LED Controller</h1>
    <p>Control your LED animations:</p>
    <a href="/mode1" class="button">Rainbow</a>
    <a href="/mode2" class="button">Chase</a>
    <!-- Add more buttons or links for additional modes or functionalities -->
</body>
</html>
)rawliteral";

#endif // HTMLPAGES_H
