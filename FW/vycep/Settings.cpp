#include "Settings.h"
Settings::Settings(SemaphoreHandle_t xSemaphore, const char* nvsParttionName) {
  this->xSemaphore = xSemaphore;
  systemSettingsStorage = new TSafePreferences(this->xSemaphore, NAMESPACE_SYSTEM_SETTINGS, nvsParttionName);
}

Settings::~Settings() {
  delete (systemSettingsStorage);
}