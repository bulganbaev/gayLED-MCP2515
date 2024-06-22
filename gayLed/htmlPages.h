//htmlPages.h
#ifndef HTMLPAGES_H
#define HTMLPAGES_H
#include <pgmspace.h>

const char HTML_HEADER[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>LED Controller</title>
    <style>
        body, html {
            margin: 0;
            padding: 0;
            overflow: hidden;
            height: 100%;
            width: 100%;
            display: flex;
            justify-content: center;
            align-items: center;
            background-color: #00d4ff;
        }
        .main-container {
            width: 70vw;
            max-width: 1000px; /* Ensures container does not get too wide on larger screens */
            height: 60vh;
            background-color: #ffc0cb;
            border-radius: 15px;
            padding: 20px;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
            display: flex;
            flex-direction: column;
            align-items: center;
        }
        .heading h1 {
            font-size: 5vh;
            color: #333;
            margin: 0 0 10px 0;
        }
        .row {
            width: 100%;
            display: flex;
            height: 20vh;
            justify-content: center;
            align-items: flex-start; /* Align items at the top */
        }
        .mode-selection p, .mode-properties p, .dropdown {
            font-size: 2.5vh;
        }
        select {
            padding: 10px;
            border-radius: 4px;
            margin: 10px 0;
            background-color: white; /* Background color of the dropdown */
            border: 2px solid #008CBA; /* Border color */
            outline: none; /* Removes the default focus outline */
            font-size: 2.5vh; /* Matching font size with other texts */
            cursor: pointer; /* Changes the cursor to indicate it's selectable */
            transition: all 0.3s ease; /* Smooth transition for interactions */
        }
        select:hover {
            background-color: #e1f5fe; /* Lighter background on hover */
        }
        select:focus {
            box-shadow: 0 0 5px 2px rgba(0, 140, 186, 0.5); /* Focus shadow */
        }
        #colorPickers {
            display: flex;
            flex-wrap: wrap;
            gap: 10px;
            margin-bottom: 10px;
        }
        .color-picker-container {
            display: flex;
            align-items: center;
        }
        .color-picker-container button {
            margin-left: 5px;
        }
        .settings-container {
            display: flex;
            align-items: center;
            margin-bottom: 15px;
        }
        .settings-container input[type=number], .settings-container select {
            width: auto; /* Adjust based on content */
            margin: 0 5px;
        }
    </style>
</head>
<body>
<div class="main-container">
)rawliteral";

const char HTML_SCRIPT[] PROGMEM = R"rawliteral(
<script>
function updateAction() {
    var mode = document.getElementById('mode').value;
    var formAction = mode == '1' ? '/obd' : '/custom'; 
    document.getElementById('modeForm').action = formAction;
    console.log(formAction);
}
</script>
)rawliteral";

const char HTML_SCRIPT_CUSTOM[] PROGMEM = R"rawliteral(
<script>
document.addEventListener('DOMContentLoaded', function() {
        // Set a random color for the initial color picker
        setRandomColor(document.querySelector('input[type="color"]'));

        document.getElementById('addColor').addEventListener('click', function() {
            var container = document.getElementById('colorPickers');
            if(container.children.length < 10) {
                var newColorPicker = document.createElement('div');
                newColorPicker.classList.add('color-picker-container');
                newColorPicker.innerHTML = '<input type="color" name="colorSet[]"><button type="button" class="removeColor">-</button>';
                container.appendChild(newColorPicker);
                // Set a random color for this new color picker
                setRandomColor(newColorPicker.querySelector('input[type="color"]'));
            } else {
                alert('You can add up to 10 colors.');
            }
        });

        document.getElementById('colorPickers').addEventListener('click', function(e) {
            if(e.target.className === 'removeColor') {
                e.target.parentNode.remove();
            }
        });
    });

    function setRandomColor(colorInput) {
        // Generate random color
        var randomColor = '#' + Math.floor(Math.random()*16777215).toString(16).padStart(6, '0');
        // Set the random color as the value
        colorInput.value = randomColor;
    }

    function adjustValue(fieldId, delta) {
        var field = document.getElementById(fieldId);
        var currentValue = parseInt(field.value);
        var newValue = currentValue + delta;

        // Respect the min and max values
        if (newValue >= parseInt(field.min) && newValue <= parseInt(field.max)) {
            field.value = newValue;
        }
}
</script>
)rawliteral";

const char HTML_FOOTER[] PROGMEM = R"rawliteral(
</div>
</body>
</html>
)rawliteral";



#endif // HTMLPAGES_H