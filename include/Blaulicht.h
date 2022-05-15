#pragma once

// 3rdparty lib includes
#include <Arduino.h>
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
        leds[index] = CRGB::Red;
        leds[(index + LED_COUNT / 2) % LED_COUNT] = CRGB::Blue;
    }

    EVERY_N_MILLIS(1)
    {
        fadeToBlackBy(leds, LED_COUNT, 1);
    }
}