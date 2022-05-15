#pragma once

// 3rdparty lib includes
#include <Arduino.h>
#include <FastLED.h>

// local includes
#include "led.h"

void FillRainbow() {
    static uint8_t hue = 0;
    EVERY_N_MILLIS(50)
    {
        CRGB color = CHSV(hue, 255, 255);
        fill_solid(leds, LED_COUNT, color);
        if (hue < 255) {
            hue++;
        } else {
            hue = 0;
        }
    }
}