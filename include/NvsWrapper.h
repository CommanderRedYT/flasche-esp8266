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
} config_t;

std::string default_ssid()
{
  char ap_ssid[22];
  snprintf(ap_ssid, 22, "GPN-LAMP-%06X", ESP.getChipId());
  return ap_ssid;
}

void save_nvs(config_t &config)
{
  EEPROM.begin(4096);
  EEPROM.put(0, config);
  Serial.println(EEPROM.commit() ? "Saved" : "Failed to save");
  EEPROM.end();
}

void load_nvs(config_t &config)
{
  EEPROM.begin(4096);
  EEPROM.get(0, config);
  EEPROM.end();

  bool used_defaults = false;

  if (strcmp(config.wifi_sta.valid, "OK\0") != 0) {
    strcpy(config.wifi_sta.ssid, "bobbycar");
    strcpy(config.wifi_sta.password, "12345678");
    Serial.println("No sta ssid found in nvs, using default");
    strcpy(config.wifi_sta.valid, "OK");
    used_defaults = used_defaults || true;
  }

  if (strcmp(config.wifi_ap.valid, "OK") != 0) {
    strcpy(config.wifi_ap.ssid, default_ssid().c_str());
    strcpy(config.wifi_ap.password, "12345678");
    Serial.println("No ap ssid found in nvs, using default");
    strcpy(config.wifi_ap.valid, "OK");
    used_defaults = used_defaults || true;
  }

  if (strcmp(config.ota_url_valid, "OK") == 0) {
    strcpy(config.ota_url, "http://lamps.bobbycar.cloud/firmwares/latest.bin");
    Serial.println("No ota url found in nvs, using default");
    strcpy(config.ota_url_valid, "OK");
    used_defaults = used_defaults || true;
  }

  if (!config.brightness.touched && config.brightness.value == 0)
  {
    config.brightness.value = 80;
    Serial.println("No brightness found in nvs, using default");
    used_defaults = used_defaults || true;
  }

  if (used_defaults)
  {
    Serial.println("Loaded, Saving defaults");
    save_nvs(config);
  }
  else
  {
    Serial.println("Loaded, No Defaults used");
  }
}

void erase_nvs()
{
  EEPROM.begin(4096);

  Serial.println("Erasing nvs");
  
  for (int i = 0; i < 4096; i++) {
    EEPROM.write(i, 0);
  }

  Serial.println(EEPROM.commit() ? "Erased" : "Failed to erase");

  EEPROM.end();
}