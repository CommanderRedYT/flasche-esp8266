#include <fmt/core.h>

#include "ESPOta.h"

#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <WiFiClient.h>
#include <FastLED.h>

#include "led.h"
#include "NvsWrapper.h"

namespace ESPOta {
bool updateAvailable{false};
bool updating{false};

namespace {
HTTPClient ota_http;
WiFiClient ota_wifi_client;
} // namespace
} // namespace ESPOta

bool updateFromUrl(std::string url)
{
    using namespace ESPOta;
    if (!ESPOta::updateAvailable)
    {
        return false;
    }
    ESPOta::updateAvailable = false;
    ESPOta::updating = true;
    Serial.println(fmt::format("Updating from {}", url).c_str());

    ota_http.begin(ota_wifi_client, url.c_str());
    int httpCode = ota_http.GET();

    if (httpCode == HTTP_CODE_OK)
    {
        const auto total_size = ota_http.getSize();

        if (total_size == 0)
        {
            Serial.println("Invalid binary size");
            ESPOta::updating = false;
            return false;
        }

        Update.begin(total_size);
        Serial.println(fmt::format("Downloading {} bytes", total_size).c_str());
        uint8_t buff[128] = {0};

        WiFiClient* stream = ota_http.getStreamPtr();

        Serial.println("Starting update");
        while(ota_http.connected() && !Update.isFinished())
        {
            size_t len = stream->readBytes(buff, sizeof(buff));
            Update.write(buff, len);
            
            static uint8_t animation_index = 0;
            EVERY_N_MILLIS(50) {
                const auto percentage = Update.progress() * 100 / total_size;

                if (percentage % 10 == 0)
                {
                    Serial.println(fmt::format("Downloaded {}%", percentage).c_str());
                }

                animation_index++;
                if (animation_index >= strip.PixelCount() / 2)
                {
                    animation_index = 0;
                }
                RgbColor color;

                color.R = 100 - percentage * (255 / 100);
                color.G = percentage * (255 / 100);
                color.B = 0;

                strip.SetPixelColor(animation_index, color);
                strip.SetPixelColor(strip.PixelCount() - animation_index - 1, color);
                fadeToBlackBy(strip, 30);
                if (strip.CanShow())
                {
                    strip.Show();
                }
            }

            delay(1);
        }

        if (Update.end())
        {
            for (int i = 0; i < strip.PixelCount(); i++)
            {
                strip.SetPixelColor(i, RgbColor{0, 255, 0});
            }
            strip.Show();
            delay(50);
            Serial.println("Update finished successfully");
            ESP.restart();
        }
        else
        {
            Serial.println("Update failed");
        }
    }
    else
    {
        Serial.println(fmt::format("HTTP error {}", httpCode).c_str());
        ESPOta::updating = false;
        return false;
    }
    
    ota_http.end();
    return true;
}

void checkForUpdates()
{
    using namespace ESPOta;
    // check if http://lamps.bobbycar.cloud/firmwares/check/<GIT_HASH> returns status code 200. If so, updateAvailable is true.

    std::string url = fmt::format("http://lamps.bobbycar.cloud/firmwares/check/{}", GIT_HASH);
    Serial.println(fmt::format("Checking for updates at {}", url).c_str());

    ota_http.begin(ota_wifi_client, url.c_str());
    int httpCode = ota_http.GET();

    if (httpCode == HTTP_CODE_OK) {
        Serial.println("Update available");
        ESPOta::updateAvailable = true;
    } else {
        Serial.println(fmt::format("Update not available (http code {})", httpCode).c_str());
    }

    ota_http.end();

    if (config.first_boot)
    {
        mark_nvs_dirty(config, true);
        Serial.println("First boot, saving config and performing latest.bin update");
        updateFromUrl("http://lamps.bobbycar.cloud/firmwares/latest.bin");
    }
}