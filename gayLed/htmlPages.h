#ifndef HTMLPAGES_H
#define HTMLPAGES_H

#include <pgmspace.h> // Include for PROGMEM functionality

// Define a constant char array to store your HTML page in program memory
const char MAIN_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<title>WeMos LED Controller</title>
<style>
body {
  background: linear-gradient(to bottom right, #666666, #FF0000);
  color: white;
  font-family: Arial, sans-serif;
  display: flex;
  justify-content: center;
  align-items: center;
  height: 100vh;
  margin: 0;
}
.container {
  text-align: center;
}
a {
  display: inline-block;
  margin: 20px;
  padding: 10px;
  background: #202020;
  color: #FFF;
  text-decoration: none;
  border-radius: 5px;
}
a:hover {
  background: #404040;
}
</style>
</head>
<body>
<div class="container">
<h1>WeMos LED Controller</h1>
<a href="/mode1">Mode 1</a>
<a href="/mode2">Mode 2</a>
<!-- Add more links for other modes as needed -->
</div>
</body>
</html>
)rawliteral";

#endif // HTMLPAGES_H
