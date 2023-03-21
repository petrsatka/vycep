#include "Settings.h"
Settings::Settings(SemaphoreHandle_t xSemaphore, const char* nvsParttionName) {
  this->xSemaphore = xSemaphore;
  systemSettingsStorage = new TSafePreferences(this->xSemaphore, NAMESPACE_SYSTEM_SETTINGS, nvsParttionName);
}

Settings::~Settings() {
  delete (systemSettingsStorage);
}

void Settings::clearAll() {
  dprintln("clearAll");
  systemSettingsStorage->clear();
}

bool Settings::setWiFiOK() {
  dprintln("setWiFiOK");
  return systemSettingsStorage->putBool(Settings::KEY_WIFI_OK, true);
}

bool Settings::clearWiFiOK() {
  dprintln("clearWiFiOK");
  return systemSettingsStorage->putBool(Settings::KEY_WIFI_OK, false);
}

bool Settings::setSSID(const char* ssid) {
  sprintln("!setSSID");
  if (ssid != NULL && strlen(ssid) < Utils::SSID_BUFFER_SIZE) {
    return systemSettingsStorage->putString(Settings::KEY_SSID, ssid) == strlen(ssid);
  }

  return false;
}

bool Settings::setSecurityKey(const char* securityKey) {
  sprintln("!setSecurityKey");
  if (securityKey != NULL && strlen(securityKey) < Utils::SECURITY_KEY_BUFFER_SIZE) {
    return systemSettingsStorage->putString(Settings::KEY_SECURITY_KEY, securityKey) == strlen(securityKey);
  }

  return false;
}

bool Settings::setPulsePerLiterCount(unsigned int pulseCount) {
  sprintln("!setPulsePerLiterCount");
  return systemSettingsStorage->putUInt(Settings::KEY_PULSE_PER_LITER, pulseCount) > 0;
}

bool Settings::setMode(Settings::DeviceMode mode) {
  sprintln("!setMode");
  return systemSettingsStorage->putShort(Settings::KEY_MODE, static_cast<short>(mode)) > 0;
}

bool Settings::setMasterTimeoutSeconds(unsigned long timeoutSeconds) {
  sprintln("!setMasterTimeoutSeconds");
  return systemSettingsStorage->putULong(Settings::KEY_MASTER_TIMEOUT, timeoutSeconds) > 0;
}

bool Settings::setUnderLimitTimeoutSeconds(unsigned long timeoutSeconds) {
  sprintln("!setUnderLimitTimeoutSeconds");
  return systemSettingsStorage->putULong(Settings::KEY_UNDER_LIMIT_TIMEOUT, timeoutSeconds) > 0;
}

bool Settings::setNewUserPaymentEnabled(bool enabled) {
  sprintln("!setNewUserPaymentEnabled");
  return systemSettingsStorage->putBool(Settings::KEY_NEW_USER_PAYMNET, enabled) > 0;
}

void Settings::getSSID(char* ssid) {
  sprintln("getSSID");
  if (ssid != NULL) {
    ssid[0] = 0;
    systemSettingsStorage->getString(Settings::KEY_SSID, ssid, Utils::SSID_BUFFER_SIZE);
  }
}

void Settings::getSecurityKey(char* securityKey) {
  sprintln("getSecurityKey");
  if (securityKey != NULL) {
    securityKey[0] = 0;
    systemSettingsStorage->getString(Settings::KEY_SECURITY_KEY, securityKey, Utils::SECURITY_KEY_BUFFER_SIZE);
  }
}

unsigned int Settings::getPulsePerLiterCount() {
  sprintln("!getPulsePerLiterCount");
  return systemSettingsStorage->getUInt(Settings::KEY_PULSE_PER_LITER, Settings::DEFAULT_PULSE_PER_LITER);
}

Settings::DeviceMode Settings::getMode() {
  sprintln("!getMode");
  return static_cast<Settings::DeviceMode>(systemSettingsStorage->getShort(Settings::KEY_MODE, static_cast<short>(Settings::DeviceMode::AUTO)));
}

unsigned long Settings::getMasterTimeoutSeconds() {
  sprintln("!getMasterTimeoutSeconds");
  return systemSettingsStorage->getULong(Settings::KEY_MASTER_TIMEOUT, Settings::DEFAULT_MASTER_TIMEOUT_SECONDS);
}

unsigned long Settings::getUnderLimitTimeoutSeconds() {
  sprintln("!getUnderLimitTimeoutSeconds");
  return systemSettingsStorage->getULong(Settings::KEY_UNDER_LIMIT_TIMEOUT, Settings::DEFAULT_UNDER_LIMIT_TIMEOUT_SECONDS);
}

bool Settings::getNewUserPaymentEnabled() {
  sprintln("!getNewUserPaymentEnabled");
  return systemSettingsStorage->getBool(Settings::KEY_NEW_USER_PAYMNET);
}

bool Settings::getWiFiOK() {
  dprintln("getWiFiOK");
  return systemSettingsStorage->getBool(Settings::KEY_WIFI_OK);
}