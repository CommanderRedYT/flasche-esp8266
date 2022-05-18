#pragma once
// esp8266 nvs wrapper header

#include <Arduino.h>
#include <string>
#include <EEPROM.h>

template<typename T>
struct NvsItem {
  T value;
  bool touched{false};
};

typedef struct {
  struct {
    char ssid[65];
    char password[33];
    char valid[3];
  } wifi_sta;
  struct {
    char ssid[65];
    char password[33];
    char valid[3];
  } wifi_ap;

  char ota_url[64];
  char ota_url_valid[3];

  NvsItem<uint8_t> brightness;
  NvsItem<uint8_t> animation;

  bool first_boot{true};
} config_t;

extern config_t config;

std::string default_ssid();

void save_nvs(config_t &config);

void load_nvs(config_t &config);

void erase_nvs(config_t &config);

void mark_nvs_dirty(config_t &config, bool erase_first = false);