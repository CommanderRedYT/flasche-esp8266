#pragma once

// 3rdparty lib includes
#include <Arduino.h>
#include <NeoPixelBus.h>
#include <FastLED.h>

// local includes
#include "led.h"


void ColorStrobo() {
    static const RgbColor colors[] = { RgbColor{255, 0, 0}, RgbColor{0, 255, 0}, RgbColor{0, 0, 255} };

    static uint8_t strobe = 0;
    EVERY_N_MILLIS(50)
    {
        fill_solid(strip, LED_COUNT, colors[strobe]);
        strobe++;
        if (strobe >= std::size(colors))
            strobe = 0;
    }
}
