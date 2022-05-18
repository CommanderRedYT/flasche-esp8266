#pragma once

// system includes
#include <Arduino.h>
#include <string>

// 3rdparty lib includes
#include <NeoPixelBrightnessBus.h>
#include <FastLED.h>

using ledstrip_t = NeoPixelBrightnessBus<NeoGrbFeature, NeoEsp8266AsyncUart1800KbpsMethod>;

extern ledstrip_t strip;

void fadeToBlackBy(ledstrip_t &strip, uint8_t fadeBy);
void fill_solid(ledstrip_t &strip, uint16_t numToFill, RgbColor color);
void fill_solid(ledstrip_t &strip, RgbColor color);
RgbColor toNeoPixelColor(CRGB color);
void fill_rainbow(ledstrip_t &strip, uint16_t startPixel, uint16_t endPixel, uint8_t initialhue, uint8_t deltahue = 1);

typedef void (*animation_t)();
typedef struct {
    animation_t animation;
    std::string name;
} animation_info_t;

extern const animation_info_t animations[];