#pragma once

// 3rdparty lib includes
#include <Arduino.h>
#include <FastLED.h>

// local includes
#include "led.h"

void FillBlue() {
    EVERY_N_MILLIS(100)
    {
        fill_solid(leds, LED_COUNT, CRGB::Blue);
    }
}