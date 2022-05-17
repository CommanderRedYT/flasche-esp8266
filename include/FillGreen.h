#pragma once

// 3rdparty lib includes
#include <Arduino.h>
#include <NeoPixelBus.h>
#include <FastLED.h>

// local includes
#include "led.h"

void FillGreen() {
    EVERY_N_MILLIS(100)
    {
        fill_solid(strip, LED_COUNT, RgbColor{0, 255, 0});
    }
}