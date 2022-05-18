#include <Arduino.h>

// system includes
#include <string>

// 3rdparty lib includes
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266WiFi.h>
#include <NeoPixelBrightnessBus.h>
#include <FastLED.h>
#include <WiFiClient.h>

// local includes
#include "ESPOta.h"
#include "led.h"
#include "NvsWrapper.h"

// animations
#include "DefaultAnimation.h"
#include "RainbowAnimation.h"
#include "Blaulicht.h"
#include "WhiteStrobo.h"
#include "ColorStrobo.h"
#include "FillRed.h"
#include "FillGreen.h"
#include "FillBlue.h"
#include "FillRainbow.h"

// webserver
#include "html.h"

WiFiClient client;
AsyncWebServer server(80);

config_t config;

ledstrip_t strip(LED_COUNT, LED_PIN);

const animation_info_t animations[] = {
  { DefaultAnimation, "Default"},
  { RainbowAnimation, "Rainbow"},
  { Blaulicht, "Blaulicht"},
  { WhiteStrobo, "WhiteStrobo"},
  { ColorStrobo, "ColorStrobo"},
  { FillRed, "FillRed"},
  { FillGreen, "FillGreen"},
  { FillBlue, "FillBlue"},
  { FillRainbow, "FillRainbow"},
};

uint8_t current_animation{0};

uint32_t block_brightness_button{0};
bool brightness_button_pressed{false};

uint32_t block_animation_button{0};
bool animation_button_pressed{false};

bool debug_print{false};
bool wifi_connected{false};
bool restart_required{false};

void setup()
{
  Serial.begin(115200);

  while (!Serial)
  {
    delay(1000);
  }
  Serial.println("Serial initialized");

  // buttons
  pinMode(PIN_ANIMATION, INPUT_PULLUP);
  pinMode(PIN_BRIGHTNESS, INPUT_PULLUP);

  // nvs
  Serial.println("Loading nvs");
  load_nvs(config);

  // led
  strip.Begin();
  fill_rainbow(strip, 0, LED_COUNT, 0, 7);
  strip.SetBrightness(config.brightness.value);
  current_animation = config.animation.value;
  strip.Show();

  // init wifi
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(config.wifi_ap.ssid, config.wifi_ap.password);

  delay(100);

  if (digitalRead(PIN_ANIMATION) || digitalRead(PIN_BRIGHTNESS))
  {
    Serial.println("Deactivate AP");
    WiFi.mode(WIFI_STA);
  }
  else
  {
    Serial.printf("AP SSID: %s\n", WiFi.softAPSSID().c_str());
  }
  Serial.println("DEBUG: Boot Successful");

  Serial.printf("Connecting to WiFi \"%s\"...\n", config.wifi_sta.ssid, config.wifi_sta.password);
  WiFi.begin(config.wifi_sta.ssid, config.wifi_sta.password);
  WiFi.setHostname(config.wifi_ap.ssid);
  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);

  // init http server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/html", html::index_html);
  });

  server.on("/reboot", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/html", html::index_html);
    restart_required = true;
  });

  server.on("/clear_nvs", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/html", html::index_html);
    erase_nvs(config);
    restart_required = true;
  });

  server.on("/settings", HTTP_GET, [](AsyncWebServerRequest* request) {
    char buffer[strlen(html::settings_html) + sizeof(config_t)];
    sprintf(buffer, html::settings_html, config.brightness.value, config.animation.value, sizeof(animations) / sizeof(animations[0]) - 1, config.wifi_sta.ssid, config.wifi_sta.password, config.wifi_ap.ssid, config.wifi_ap.password, config.ota_url);
    request->send(200, "text/html", buffer);
  });

  server.on("/brightness", HTTP_GET, [](AsyncWebServerRequest* request) {
    // set brightness, save nvs and redirect to /settings

    if (request->hasParam("brightness"))
    {
      const auto decoded = request->getParam("brightness")->value().toInt();
      config.brightness.value = decoded;
      strip.SetBrightness(config.brightness.value);
      save_nvs(config);
    }

    request->redirect("/settings");
  });

  server.on("/animation", HTTP_GET, [](AsyncWebServerRequest* request) {
    // set animation, save nvs and redirect to /settings

    if (request->hasParam("animation"))
    {
      const auto decoded = request->getParam("animation")->value().toInt();
      current_animation = decoded;
      config.animation.value = decoded;
      save_nvs(config);
    }

    request->redirect("/settings");
  });

  server.on("/sta_ssid", HTTP_GET, [](AsyncWebServerRequest* request) {
    // set sta_ssid, save nvs and redirect to /settings

    if (request->hasParam("sta_ssid"))
    {
      const auto decoded = request->getParam("sta_ssid")->value().c_str();
      strcpy(config.wifi_sta.ssid, decoded);
      save_nvs(config);
    }

    request->redirect("/settings");
    restart_required = true;
  });

  server.on("/sta_password", HTTP_GET, [](AsyncWebServerRequest* request) {
    // set sta_password, save nvs and redirect to /settings

    if (request->hasParam("sta_password"))
    {
      const auto decoded = request->getParam("sta_password")->value().c_str();
      strcpy(config.wifi_sta.password, decoded);
      save_nvs(config);
    }

    request->redirect("/settings");
    restart_required = true;
  });

  server.on("/ap_ssid", HTTP_GET, [](AsyncWebServerRequest* request) {
    // set ap_ssid, save nvs and redirect to /settings

    if (request->hasParam("ap_ssid"))
    {
      const auto decoded = request->getParam("ap_ssid")->value().c_str();
      strcpy(config.wifi_ap.ssid, decoded);
      save_nvs(config);
    }

    request->redirect("/settings");
    restart_required = true;
  });

  server.on("/ap_password", HTTP_GET, [](AsyncWebServerRequest* request) {
    // set ap_password, save nvs and redirect to /settings

    if (request->hasParam("ap_password"))
    {
      const auto decoded = request->getParam("ap_password")->value().c_str();
      strcpy(config.wifi_ap.password, decoded);
      save_nvs(config);
    }

    request->redirect("/settings");
    restart_required = true;
  });

  server.on("/manual_update", HTTP_GET, [](AsyncWebServerRequest* request) {
    // update firmware

    if (request->hasParam("url"))
    {
      const auto decoded = request->getParam("url")->value().c_str();
      Serial.printf("Update firmware from %s\n", decoded);
      updateFromUrl(decoded);
    }

    request->redirect("/");
  });

  server.on("/update_url", HTTP_GET, [](AsyncWebServerRequest* request) {
    // set update url, save nvs and redirect to /settings

    if (request->hasParam("update_url"))
    {
      const auto decoded = request->getParam("update_url")->value().c_str();
      strcpy(config.ota_url, decoded);
      save_nvs(config);
    }

    request->redirect("/settings");
  });

  server.begin();

  // mdns
  if (MDNS.begin(config.wifi_ap.ssid))
  {
    Serial.println("mDNS responder started");
    MDNS.addService("http", "tcp", 80);
  }
  else
  {
    Serial.println("Error setting up MDNS responder!");
  }
}

void nextBrightness()
{
  if (config.brightness.value > 3)
    config.brightness.value /= 2;
  else if (config.brightness.value == 0)
    config.brightness.value = 255;
  else
    config.brightness.value = 0;

  strip.SetBrightness(config.brightness.value);
  save_nvs(config);
  strip.Show();
}

void handleLeds()
{
  // check if current_animation is in bounds, if not, set to 0
  if (current_animation >= sizeof(animations) / sizeof(animations[0]))
  {
    current_animation = 0;
    config.animation.value = 0;
    save_nvs(config);
  }
  if (ESPOta::updateAvailable)
  {
    static bool update_blink = false;
    // leds 1-17 and all others red
    EVERY_N_MILLIS(100) {
      for (uint32_t i = 0; i < strip.PixelCount(); i++)
      {
        strip.SetPixelColor(i, i < 16 ? RgbColor{0, 255, 0} : RgbColor{255, 0, 0}); // animation = red, brightness = green
      }

      if (update_blink)
      {
        for (uint32_t i = 7; i <= 9; i++)
        {
          strip.SetPixelColor(i, RgbColor{255, 255, 0});
        }
        for (uint32_t i = 23; i <= 25; i++)
        {
          strip.SetPixelColor(i, RgbColor{255, 255, 0});
        }
      }
    }

    EVERY_N_MILLIS(500) {
      update_blink = !update_blink;
    }
  }
  else
  {
    animations[current_animation].animation();
  }
}

void handleButtons()
{
  // brightness button
  const auto now = millis();
  const bool brightness_button_pressed_now = !digitalRead(PIN_BRIGHTNESS);
  if (brightness_button_pressed_now && !brightness_button_pressed && now > block_brightness_button)
  {
    block_brightness_button = now + BUTTON_DEBOUNCE_DELAY;

    if (ESPOta::updateAvailable)
    {
      Serial.printf("Updating firmware (%s)\n", config.ota_url);
      updateFromUrl(config.ota_url);
    }
    else if (ESPOta::updating)
    {

    }
    else
    {
      nextBrightness();
    }

    brightness_button_pressed = brightness_button_pressed_now;
  }
  else if (!brightness_button_pressed_now && brightness_button_pressed && now > block_brightness_button)
  {
    block_brightness_button = now + BUTTON_DEBOUNCE_DELAY;
    brightness_button_pressed = brightness_button_pressed_now;
  }

  // animation button
  const bool animation_button_pressed_now = !digitalRead(PIN_ANIMATION);
  if (animation_button_pressed_now && !animation_button_pressed && now > block_animation_button)
  {
    block_animation_button = now + BUTTON_DEBOUNCE_DELAY;

    if (ESPOta::updateAvailable)
    {
      ESPOta::updateAvailable = false;
      Serial.println("Cancelling firmware update");
    }
    else if (ESPOta::updating)
    {
      // do nothing
    }
    else
    {
      current_animation++;
      config.animation.value = current_animation;
      save_nvs(config);
    }
    animation_button_pressed = animation_button_pressed_now;
  }
  else if (!animation_button_pressed_now && animation_button_pressed && now > block_animation_button)
  {
    block_animation_button = now + BUTTON_DEBOUNCE_DELAY;
    animation_button_pressed = animation_button_pressed_now;
  }
}

void loop()
{
  handleButtons();
  handleLeds();

  if (!wifi_connected && WiFi.status() == WL_CONNECTED)
  {
    wifi_connected = true;
    Serial.printf("Connected to WiFi. Serving at http://%s\n", WiFi.localIP().toString().c_str());
    checkForUpdates();
  }

  if (strip.CanShow())
  {
    strip.Show();
    if (!debug_print)
    {
      Serial.println("DEBUG: Render successful");
      debug_print = true;
    }
  }

  if (restart_required)
  {
    Serial.println("Restarting...");
    delay(1000);
    ESP.restart();
  }
}
