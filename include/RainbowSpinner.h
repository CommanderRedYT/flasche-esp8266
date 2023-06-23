#pragma once

// 3rdparty lib includes
#include <Arduino.h>
#include <NeoPixelBus.h>
#include <FastLED.h>

// local includes
#include "led.h"

void RainbowSpinner() {
    static uint8_t position = 0;
    static uint8_t hue = 0;

    EVERY_N_MILLIS(7)
    {
        fill_rainbow(strip, position, (position + 1) % LED_COUNT + 1, hue, 1);
        position++;
        
        if (position > LED_COUNT) {
            position = 0;
        }

        hue-=2;

        fadeToBlackBy(strip, 15);
    }
}