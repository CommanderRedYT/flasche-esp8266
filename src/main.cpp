#include <Arduino.h>

// 3rdparty lib includes
#include <FastLED.h>
#include <ESP8266WiFi.h>

// local includes
#include "led.h"

// animations
#include "DefaultAnimation.h"
#include "RainbowAnimation.h"
#include "Blaulicht.h"
#include "WhiteStrobo.h"
#include "ColorStrobo.h"
#include "FillRed.h"
#include "FillGreen.h"
#include "FillBlue.h"
#include "FillRainbow.h"

CRGB leds[LED_COUNT];

const animation_info_t animations[] = {
  { DefaultAnimation, "Default"},
  { RainbowAnimation, "Rainbow"},
  { Blaulicht, "Blaulicht"},
  { WhiteStrobo, "WhiteStrobo"},
  { ColorStrobo, "ColorStrobo"},
  { FillRed, "FillRed"},
  { FillGreen, "FillGreen"},
  { FillBlue, "FillBlue"},
  { FillRainbow, "FillRainbow"},
};

uint8_t current_animation = 0;
uint8_t brightness = 255;

uint32_t block_brightness_button = 0;
bool brightness_button_pressed = false;

uint32_t block_animation_button = 0;
bool animation_button_pressed = false;

void setup()
{

  Serial.begin(115200);

  // buttons
  pinMode(PIN_ANIMATION, INPUT_PULLUP);
  pinMode(PIN_BRIGHTNESS, INPUT_PULLUP);

  // init fastled
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, LED_COUNT);
  fill_rainbow(leds, LED_COUNT, 0);
  FastLED.setBrightness(255);
  FastLED.show();
  delay(100);
}

void nextBrightness()
{
  if (brightness > 3)
    brightness /= 2;
  else if (brightness == 0)
    brightness = 255;
  else
    brightness = 0;

  FastLED.setBrightness(brightness);
  FastLED.show();
}

void handleLeds()
{
  // check if current_animation is in bounds, if not, set to 0
  if (current_animation >= sizeof(animations) / sizeof(animations[0]))
  {
    current_animation = 0;
  }
  animations[current_animation].animation();
}

void handleButtons()
{
  // brightness button
  const auto now = millis();
  const bool brightness_button_pressed_now = !digitalRead(PIN_BRIGHTNESS);
  if (brightness_button_pressed_now && !brightness_button_pressed && now > block_brightness_button)
  {
    block_brightness_button = now + BUTTON_DEBOUNCE_DELAY;
    nextBrightness();
    brightness_button_pressed = brightness_button_pressed_now;
  }
  else if (!brightness_button_pressed_now && brightness_button_pressed && now > block_brightness_button)
  {
    block_brightness_button = now + BUTTON_DEBOUNCE_DELAY;
    brightness_button_pressed = brightness_button_pressed_now;
  }

  // animation button
  const bool animation_button_pressed_now = !digitalRead(PIN_ANIMATION);
  if (animation_button_pressed_now && !animation_button_pressed && now > block_animation_button)
  {
    block_animation_button = now + BUTTON_DEBOUNCE_DELAY;
    current_animation++;
    animation_button_pressed = animation_button_pressed_now;
  }
  else if (!animation_button_pressed_now && animation_button_pressed && now > block_animation_button)
  {
    block_animation_button = now + BUTTON_DEBOUNCE_DELAY;
    animation_button_pressed = animation_button_pressed_now;
  }
}

void loop()
{
  handleButtons();
  handleLeds();
  EVERY_N_MILLIS(50) // render every 50ms
  {
    FastLED.show();
  }
}
