#pragma once

// 3rdparty lib includes
#include <Arduino.h>
#include <NeoPixelBus.h>
#include <FastLED.h>

// local includes
#include "led.h"

void WhiteStrobo() {
    static bool strobe = false;
    EVERY_N_MILLIS(50)
    {
        fill_solid(strip, LED_COUNT, strobe ? RgbColor{255, 255, 255} : RgbColor{0, 0, 0});
        strobe = !strobe;
    }
}