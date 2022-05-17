#pragma once

// system includes
#include <Arduino.h>

// 3rdparty lib includes
#include <NeoPixelBus.h>
#include <FastLED.h>

// local includes
#include "led.h"

void RainbowAnimation() {
    static uint16_t index = 0;

    EVERY_N_MILLIS(20)
    {
        if (index++ > 255) {
            index = 0;
        }
        fill_rainbow(strip, 0, LED_COUNT, index, 7);
    }
}