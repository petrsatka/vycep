#include "Settings.h"
Settings::Settings(SemaphoreHandle_t xSemaphore, const char* nvsParttionName) {
  this->xSemaphore = xSemaphore;
  systemSettingsStorage = new TSafePreferences(this->xSemaphore, NAMESPACE_SYSTEM_SETTINGS, nvsParttionName);
}

Settings::~Settings() {
  delete (systemSettingsStorage);
}

void Settings::clearAll() {
  systemSettingsStorage->clear();
}

bool Settings::setSSID(const char* ssid) {
  if (ssid != NULL && strlen(ssid) < Utils::SSID_BUFFER_SIZE) {
    return systemSettingsStorage->putString(Settings::KEY_SSID, ssid) == strlen(ssid) + 1;
  }

  return false;
}

bool Settings::setSecurityKey(const char* securityKey) {
  if (securityKey != NULL && strlen(securityKey) < Utils::SECURITY_KEY_BUFFER_SIZE) {
    return systemSettingsStorage->putString(Settings::KEY_SECURITY_KEY, securityKey) == strlen(securityKey) + 1;
  }

  return false;
}

bool Settings::setPulsePerLiterCount(unsigned int pulseCount) {
  return systemSettingsStorage->putUInt(Settings::KEY_PULSE_PER_LITER, pulseCount) > 0;
}

bool Settings::setMode(Settings::DeviceMode mode) {
  return systemSettingsStorage->putShort(Settings::KEY_MODE, static_cast<short>(mode)) > 0;
}

bool Settings::setMasterTimeoutSeconds(unsigned long timeoutSeconds) {
  return systemSettingsStorage->putULong(Settings::KEY_MASTER_TIMEOUT, timeoutSeconds) > 0;
}

bool Settings::setUnderLimitTimeoutSeconds(unsigned long timeoutSeconds) {
  return systemSettingsStorage->putULong(Settings::KEY_UNDER_LIMIT_TIMEOUT, timeoutSeconds) > 0;
}

void Settings::getSSID(char* ssid) {
  if (ssid != NULL) {
    ssid[0] = 0;
    systemSettingsStorage->getString(Settings::KEY_SSID, ssid, Utils::SSID_BUFFER_SIZE);
  }
}

void Settings::getSecurityKey(char* securityKey) {
  if (securityKey != NULL) {
    securityKey[0] = 0;
    systemSettingsStorage->getString(Settings::KEY_SECURITY_KEY, securityKey, Utils::SECURITY_KEY_BUFFER_SIZE);
  }
}

unsigned int Settings::getPulsePerLiterCount() {
  return systemSettingsStorage->getUInt(Settings::KEY_PULSE_PER_LITER, Settings::DEFAULT_PULSE_PER_LITER);
}

Settings::DeviceMode Settings::getMode() {
  return static_cast<Settings::DeviceMode>(systemSettingsStorage->getShort(Settings::KEY_MODE, static_cast<short>(Settings::DeviceMode::AUTO)));
}

unsigned long Settings::getMasterTimeoutSeconds() {
  return systemSettingsStorage->getULong(Settings::KEY_MASTER_TIMEOUT, Settings::DEFAULT_MASTER_TIMEOUT_SECONDS);
}

unsigned long Settings::getUnderLimitTimeoutSeconds() {
  return systemSettingsStorage->putULong(Settings::KEY_UNDER_LIMIT_TIMEOUT, Settings::DEFAULT_UNDER_LIMIT_TIMEOUT_SECONDS) > 0;
}