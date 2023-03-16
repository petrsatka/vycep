#ifndef Settings_h
#define Settings_h
#include <Arduino.h>
#include "TSafePreferences.h"
#include "Utils.h"
#include "Debug.h"
class Settings {
public:
  Settings(SemaphoreHandle_t xSemaphore, const char* nvsParttionName);
  ~Settings();

private:
  static constexpr const char* NAMESPACE_SYSTEM_SETTINGS = "setts-system";

  SemaphoreHandle_t xSemaphore = NULL;
  TSafePreferences* systemSettingsStorage = NULL;
};
#endif