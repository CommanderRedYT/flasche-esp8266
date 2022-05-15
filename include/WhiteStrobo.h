#pragma once

// 3rdparty lib includes
#include <Arduino.h>
#include <FastLED.h>

// local includes
#include "led.h"

void WhiteStrobo() {
    static bool strobe = false;
    EVERY_N_MILLIS(50)
    {
        fill_solid(leds, LED_COUNT, strobe ? CRGB::White : CRGB::Black);
        strobe = !strobe;
    }
}