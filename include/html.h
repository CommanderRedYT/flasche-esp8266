#pragma once
#include <Arduino.h>

// html code should take body and return header + body + footer
#define HTML_CODE(x) \
    "<!DOCTYPE html>" \
    "<html>" \
    "<head>" \
    "    <meta charset=\"utf-8\">" \
    "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">" \
    "    <title>ESP8266 LED Ring</title>"\
    "</head>"\
    "<style>"\
    "    footer {"\
    "        bottom: 0;"\
    "        position: fixed;"\
    "        width: 100%;"\
    "        padding-bottom: 10px;"\
    "    }"\
    "    html {"\
    "        font-family: Arial, Helvetica, sans-serif;"\
    "    }"\
    "</style>" \
    "<body>" \
    x \
    "<footer>" \
        "<hr>" \
        "<a href=\"https://github.com/bobbycar-graz/flasche-esp8266\">View on GitHub</a>" \
        "Special thanks to <a href=\"https://github.com/CommanderRedYT\">CommanderRedYT</a> for programming the ESP8266 and" \
        "<a href=\"https://github.com/mickdermack\">mickdermack</a> for providing animations and fixing the code!" \
    "</footer>" \
    "</body></html>"

namespace html {
const static char *index_html PROGMEM = HTML_CODE(R"=====(
    <h1>ESP8266 LED Ring</h1>
    <h3>Thanks for using this project!</h3>
    <p>
        This project is a simple LED ring that can be controlled via a web interface.
        The following functions are available:
    </p>
    <ul>
        <li>
            <a href="/settings">Settings</a>
        </li>
    </ul>
)=====");

const static char *settings_html PROGMEM = HTML_CODE(R"=====(
    <h1>Settings</h1>
    <p>
        Here you can change the settings of the LED ring.
    </p>
    <hr>
    <form action="/brightness">
        <label for="brightness">Brightness:</label>
        <input type="number" name="brightness" value="%d" min="0" max="255" step="1" id="brightness" />
        <input type="submit" value="Save" />
    </form>
    <form action="/animation">
        <label for="animation">Animation:</label>
        <input type="number" name="animation" value="%d" min="0" max="%d" step="1" id="animation" />
        <input type="submit" value="Save" />
    </form>
    <hr>
    <form action="/sta_ssid">
        <label for="sta_ssid">WiFi SSID:</label>
        <input type="text" name="sta_ssid" value="%s" id="sta_ssid" />
        <input type="submit" value="Save" />
    </form>
    <form action="/sta_password">
        <label for="sta_password">WiFi Password:</label>
        <input type="text" name="sta_password" value="%s" id="sta_password" />
        <input type="submit" value="Save" />
    </form>
    <hr>
    <form action="/ap_ssid">
        <label for="ap_ssid">WiFi AP SSID:</label>
        <input type="text" name="ap_ssid" value="%s" id="ap_ssid" />
        <input type="submit" value="Save" />
    </form>
    <form action="/ap_password">
        <label for="ap_password">WiFi AP Password:</label>
        <input type="text" name="ap_password" value="%s" id="ap_password" />
        <input type="submit" value="Save" />
    </form>
)=====");

// how to use settings html %d:
} // namespace html