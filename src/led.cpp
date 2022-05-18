#include "led.h"

void fadeToBlackBy(ledstrip_t &strip, uint8_t fadeBy)
{
    for (uint16_t i = 0; i < strip.PixelCount(); i++)
    {
        auto color = strip.GetPixelColor(i);
        color.R = max(0, color.R - fadeBy);
        color.G = max(0, color.G - fadeBy);
        color.B = max(0, color.B - fadeBy);
        strip.SetPixelColor(i, color);
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

void fill_rainbow(ledstrip_t &strip, uint16_t startPixel, uint16_t endPixel, uint8_t initialhue, uint8_t deltahue)
{
    uint8_t hue = initialhue;
    for (uint16_t i = startPixel; i < endPixel; i++)
    {
        strip.SetPixelColor(i, toNeoPixelColor(CHSV(hue, 255, 255)));
        hue += deltahue;
    }
}