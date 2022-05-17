#pragma once

// system includes
#include <Arduino.h>
#include <string>

// 3rdparty lib includes
#include <NeoPixelBrightnessBus.h>
#include <FastLED.h>

using ledstrip_t = NeoPixelBrightnessBus<NeoGrbFeature, NeoEsp8266AsyncUart1800KbpsMethod>;

extern ledstrip_t strip;

void fadeToBlackBy(ledstrip_t &strip, uint8_t fadeBy)
{
    for (uint16_t i = 0; i < strip.PixelCount(); i++)
    {
        strip.GetPixelColor(i).Darken(fadeBy);
    }
}

void fill_solid(ledstrip_t &strip, uint16_t numToFill, RgbColor color)
{
    for (uint16_t i = 0; i < std::min(strip.PixelCount(), numToFill); i++)
    {
        strip.SetPixelColor(i, color);
    }
}

void fill_solid(ledstrip_t &strip, RgbColor color)
{
    fill_solid(strip, strip.PixelCount(), color);
}

RgbColor toNeoPixelColor(CRGB color)
{
    return RgbColor{color.r, color.g, color.b};
}

void fill_rainbow(ledstrip_t &strip, uint16_t startPixel, uint16_t endPixel, uint8_t initialhue, uint8_t deltahue = 1)
{
    uint8_t hue = initialhue;
    for (uint16_t i = startPixel; i < endPixel; i++)
    {
        strip.SetPixelColor(i, toNeoPixelColor(CHSV(hue, 255, 255)));
        hue += deltahue;
    }
}

typedef void (*animation_t)();
typedef struct {
    animation_t animation;
    std::string name;
} animation_info_t;

extern const animation_info_t animations[];