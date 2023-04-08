/*
By Petr Satka
Licnese CC-BY
*/

//Konfigurace zařízení
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
  dprintln("setSSID");
  if (ssid != NULL && strlen(ssid) < Utils::SSID_BUFFER_SIZE) {
    return systemSettingsStorage->putString(Settings::KEY_SSID, ssid) == strlen(ssid);
  }

  return false;
}

bool Settings::setSecurityKey(const char* securityKey) {
  dprintln("setSecurityKey");
  if (securityKey != NULL && strlen(securityKey) < Utils::SECURITY_KEY_BUFFER_SIZE) {
    return systemSettingsStorage->putString(Settings::KEY_SECURITY_KEY, securityKey) == strlen(securityKey);
  }

  return false;
}

bool Settings::setMdnsHostname(const char* mdnsHostname) {
  sprintln("!setMdnsHostname");
  if (mdnsHostname != NULL && strlen(mdnsHostname) < Settings::MDNS_HOSTNAME_BUFFER_SIZE) {
    return systemSettingsStorage->putString(Settings::KEY_MDNS_HOSTNAME, mdnsHostname) == strlen(mdnsHostname);
  }

  return false;
}

bool Settings::setDdnsDomain(const char* ddnsDomain) {
  sprintln("!setDdnsDomain");
  if (ddnsDomain != NULL && strlen(ddnsDomain) < DDNSS_DOAMIN_BUFFER_SIZE) {
    return systemSettingsStorage->putString(Settings::KEY_DDNS_DOMAIN, ddnsDomain) == strlen(ddnsDomain);
  }

  return false;
}

bool Settings::setDdnsUsername(const char* ddnsUsername) {
  sprintln("!setDdnsUsername");
  if (ddnsUsername != NULL && strlen(ddnsUsername) < DDNSS_USERNAME_BUFFER_SIZE) {
    return systemSettingsStorage->putString(Settings::KEY_DDNS_USERNAME, ddnsUsername) == strlen(ddnsUsername);
  }

  return false;
}

bool Settings::setDdnsPassword(const char* ddnsPassword) {
  sprintln("!setDdnsPassword");
  if (ddnsPassword != NULL && strlen(ddnsPassword) < DDNSS_PASSWORD_BUFFER_SIZE) {
    return systemSettingsStorage->putString(Settings::KEY_DDNS_PASSWORD, ddnsPassword) == strlen(ddnsPassword);
  }

  return false;
}

bool Settings::setPulsePerLiterCount(unsigned int pulseCount) {
  dprintln("setPulsePerLiterCount");
  return systemSettingsStorage->putUInt(Settings::KEY_PULSE_PER_LITER, pulseCount) > 0;
}

bool Settings::setMode(Settings::DeviceMode mode) {
  dprintln("setMode");
  return systemSettingsStorage->putShort(Settings::KEY_MODE, static_cast<short>(mode)) > 0;
}

bool Settings::setMasterTimeoutSeconds(unsigned long timeoutSeconds) {
  dprintln("setMasterTimeoutSeconds");
  return systemSettingsStorage->putULong(Settings::KEY_MASTER_TIMEOUT, timeoutSeconds) > 0;
}

bool Settings::setUnderLimitTimeoutSeconds(unsigned long timeoutSeconds) {
  dprintln("setUnderLimitTimeoutSeconds");
  return systemSettingsStorage->putULong(Settings::KEY_UNDER_LIMIT_TIMEOUT, timeoutSeconds) > 0;
}

bool Settings::setNewUserPaymentEnabled(bool enabled) {
  dprintln("setNewUserPaymentEnabled");
  return systemSettingsStorage->putBool(Settings::KEY_NEW_USER_PAYMNET, enabled) > 0;
}

void Settings::getSSID(char* ssid) {
  dprintln("getSSID");
  if (ssid != NULL) {
    ssid[0] = 0;
    systemSettingsStorage->getString(Settings::KEY_SSID, ssid, Utils::SSID_BUFFER_SIZE);
  }
}

void Settings::getMdnsHostname(char* mdnsHostname) {
  dprintln("getMdnsHostname");
  if (mdnsHostname != NULL) {
    mdnsHostname[0] = 0;
    systemSettingsStorage->getString(Settings::KEY_MDNS_HOSTNAME, mdnsHostname, Settings::MDNS_HOSTNAME_BUFFER_SIZE);
    if (mdnsHostname[0] == 0) {
      strcpy(mdnsHostname, Utils::DEFAULT_HOST_NAME);
    }
  }
}

void Settings::getDdnsDomain(char* ddnsDomain) {
  dprintln("getDdnsDomain");
  if (ddnsDomain != NULL) {
    ddnsDomain[0] = 0;
    systemSettingsStorage->getString(Settings::KEY_DDNS_DOMAIN, ddnsDomain, Settings::DDNSS_DOAMIN_BUFFER_SIZE);
  }
}

void Settings::getDdnsUsername(char* ddnsUsername) {
  dprintln("getDdnsUsername");
  if (ddnsUsername != NULL) {
    ddnsUsername[0] = 0;
    systemSettingsStorage->getString(Settings::KEY_DDNS_USERNAME, ddnsUsername, Settings::DDNSS_USERNAME_BUFFER_SIZE);
  }
}

void Settings::getDdnsPassword(char* ddnsPassword) {
  sprintln("!getDdnsPassword");
  if (ddnsPassword != NULL) {
    ddnsPassword[0] = 0;
    systemSettingsStorage->getString(Settings::KEY_DDNS_PASSWORD, ddnsPassword, Settings::DDNSS_PASSWORD_BUFFER_SIZE);
  }
}

void Settings::getSecurityKey(char* securityKey) {
  dprintln("getSecurityKey");
  if (securityKey != NULL) {
    securityKey[0] = 0;
    systemSettingsStorage->getString(Settings::KEY_SECURITY_KEY, securityKey, Utils::SECURITY_KEY_BUFFER_SIZE);
  }
}

unsigned int Settings::getPulsePerLiterCount() {
  dprintln("getPulsePerLiterCount");
  return systemSettingsStorage->getUInt(Settings::KEY_PULSE_PER_LITER, Settings::DEFAULT_PULSE_PER_LITER);
}

Settings::DeviceMode Settings::getMode() {
  dprintln("getMode");
  return static_cast<Settings::DeviceMode>(systemSettingsStorage->getShort(Settings::KEY_MODE, static_cast<short>(Settings::DeviceMode::AUTO)));
}

unsigned long Settings::getMasterTimeoutSeconds() {
  dprintln("getMasterTimeoutSeconds");
  return systemSettingsStorage->getULong(Settings::KEY_MASTER_TIMEOUT, Settings::DEFAULT_MASTER_TIMEOUT_SECONDS);
}

unsigned long Settings::getUnderLimitTimeoutSeconds() {
  dprintln("getUnderLimitTimeoutSeconds");
  return systemSettingsStorage->getULong(Settings::KEY_UNDER_LIMIT_TIMEOUT, Settings::DEFAULT_UNDER_LIMIT_TIMEOUT_SECONDS);
}

bool Settings::getNewUserPaymentEnabled() {
  dprintln("getNewUserPaymentEnabled");
  return systemSettingsStorage->getBool(Settings::KEY_NEW_USER_PAYMNET);
}

bool Settings::getWiFiOK() {
  dprintln("getWiFiOK");
  return systemSettingsStorage->getBool(Settings::KEY_WIFI_OK);
}