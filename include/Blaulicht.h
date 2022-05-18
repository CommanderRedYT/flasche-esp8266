#pragma once

// 3rdparty lib includes
#include <Arduino.h>
#include <NeoPixelBus.h>
#include <FastLED.h>

// local includes
#include "led.h"

void Blaulicht() {
    static uint16_t index = 0;
    // red light at the beginning, blue LED_COUNT / 2 after (% LED_COUNT); fade to black
    EVERY_N_MILLIS(15)
    {
        if (index++ > LED_COUNT) {
            index = 0;
        }
        strip.SetPixelColor(index, RgbColor{255, 0, 0});
        strip.SetPixelColor((index + LED_COUNT / 2) % LED_COUNT, RgbColor{0, 0, 255});
        fadeToBlackBy(strip, 30);
    }
}