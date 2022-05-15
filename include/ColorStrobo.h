#pragma once

// 3rdparty lib includes
#include <Arduino.h>
#include <FastLED.h>

// local includes
#include "led.h"


void ColorStrobo() {
    static const CRGB colors[] = { CRGB::Red, CRGB::Green, CRGB::Blue };

    static int strobe = 0;
    EVERY_N_MILLIS(50)
    {
        fill_solid(leds, LED_COUNT, colors[strobe]);
        strobe++;
        if (strobe >= std::size(colors))
            strobe = 0;
    }
}
