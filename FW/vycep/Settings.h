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

  enum class DeviceMode {
    AUTO = 0,
    OPEN = 1,
    CLOSED = 2,
  };

  void clearAll();
  bool setWiFiOK();
  bool clearWiFiOK();
  bool setSSID(const char* ssid);
  bool setSecurityKey(const char* securityKey);
  bool setPulsePerLiterCount(unsigned int pulseCount);
  bool setMode(Settings::DeviceMode mode);
  bool setMasterTimeoutSeconds(unsigned long timeoutSeconds);
  bool setUnderLimitTimeoutSeconds(unsigned long timeoutSeconds);
  void getSSID(char* ssid);
  void getSecurityKey(char* securityKey);
  unsigned int getPulsePerLiterCount();
  Settings::DeviceMode getMode();
  unsigned long getMasterTimeoutSeconds();
  unsigned long getUnderLimitTimeoutSeconds();
   bool getWiFiOK();

private:
  static constexpr const char* NAMESPACE_SYSTEM_SETTINGS = "setts-system";
  static constexpr const char* KEY_WIFI_OK = "wifiok";
  static constexpr const char* KEY_SSID = "ssid";
  static constexpr const char* KEY_SECURITY_KEY = "skey";
  static constexpr const char* KEY_PULSE_PER_LITER = "pulseplitter";
  static constexpr const char* KEY_MODE = "mode";
  static constexpr const char* KEY_MASTER_TIMEOUT = "mastertimeout";
  static constexpr const char* KEY_UNDER_LIMIT_TIMEOUT = "ulimtimeout";
  static constexpr unsigned int DEFAULT_PULSE_PER_LITER = 5880;
  static constexpr unsigned long DEFAULT_MASTER_TIMEOUT_SECONDS = 20 * 60;
  static constexpr unsigned long DEFAULT_UNDER_LIMIT_TIMEOUT_SECONDS = 10 * 60;

  SemaphoreHandle_t xSemaphore = NULL;
  TSafePreferences* systemSettingsStorage = NULL;
};
#endif