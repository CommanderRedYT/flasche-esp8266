#pragma once

#include <string>

#include <Arduino.h>

namespace ESPOta {
extern bool updateAvailable;
extern bool updating;
} // namespace ESPOta

bool updateFromUrl(std::string url);
void checkForUpdates();