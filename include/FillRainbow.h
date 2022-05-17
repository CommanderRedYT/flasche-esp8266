#pragma once

// 3rdparty lib includes
#include <Arduino.h>
#include <NeoPixelBus.h>
#include <FastLED.h>

// local includes
#include "led.h"

void FillRainbow() {
    static uint8_t hue = 0;
    EVERY_N_MILLIS(50)
    {
        fill_solid(strip, LED_COUNT, toNeoPixelColor(CHSV(hue, 255, 255)));
        if (hue < 255) {
            hue++;
        } else {
            hue = 0;
        }
    }
}