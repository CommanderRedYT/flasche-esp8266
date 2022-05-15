#pragma once

// system includes
#include <Arduino.h>
#include <string>

// 3rdparty lib includes
#include <FastLED.h>

extern CRGB leds[LED_COUNT];

typedef void (*animation_t)();
typedef struct {
    animation_t animation;
    std::string name;
} animation_info_t;

extern const animation_info_t animations[];