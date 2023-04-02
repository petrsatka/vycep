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

  static constexpr size_t DDNSS_DOAMIN_BUFFER_SIZE = 254;   //253 Znaků
  static constexpr size_t DDNSS_USERNAME_BUFFER_SIZE = 64;  //63 Znaků
  static constexpr size_t DDNSS_PASSWORD_BUFFER_SIZE = 64;  //63 Znaků

  static constexpr const char* KEY_SSID = "ssid";
  static constexpr const char* KEY_SECURITY_KEY = "skey";
  static constexpr const char* KEY_PULSE_PER_LITER = "pulseplitter";
  static constexpr const char* KEY_MODE = "mode";
  static constexpr const char* KEY_MASTER_TIMEOUT = "mastertimeout";
  static constexpr const char* KEY_UNDER_LIMIT_TIMEOUT = "ulimtimeout";
  static constexpr const char* KEY_NEW_USER_PAYMNET = "nuserpaymnt";
  static constexpr const char* KEY_ADMIN_PERMISSIONS = "admin";
  static constexpr const char* KEY_PAYMENT_PERMISSIONS = "payment";
  static constexpr const char* KEY_DDNS_DOMAIN = "ddnsdomain";
  static constexpr const char* KEY_DDNS_USERNAME = "ddnsuname";
  static constexpr const char* KEY_DDNS_PASSWORD = "ddnspassword";

  void clearAll();
  bool setWiFiOK();
  bool clearWiFiOK();
  bool setSSID(const char* ssid);
  bool setSecurityKey(const char* securityKey);
  bool setNewUserPaymentEnabled(bool enabled);
  bool setPulsePerLiterCount(unsigned int pulseCount);
  bool setMode(Settings::DeviceMode mode);
  bool setMasterTimeoutSeconds(unsigned long timeoutSeconds);
  bool setUnderLimitTimeoutSeconds(unsigned long timeoutSeconds);
  bool setDdnsDomain(const char* ddnsDomain);
  bool setDdnsUsername(const char* ddnsUsername);
  bool setDdnsPassword(const char* ddnsPassword);
  void getSSID(char* ssid);
  void getSecurityKey(char* securityKey);
  void getDdnsDomain(char* ddnsDomain);
  void getDdnsUsername(char* ddnsUsername);
  unsigned int getPulsePerLiterCount();
  Settings::DeviceMode getMode();
  unsigned long getMasterTimeoutSeconds();
  unsigned long getUnderLimitTimeoutSeconds();
  bool getNewUserPaymentEnabled();
  bool getWiFiOK();

private:
  static constexpr const char* NAMESPACE_SYSTEM_SETTINGS = "setts-system";
  static constexpr const char* KEY_WIFI_OK = "wifiok";
  static constexpr unsigned int DEFAULT_PULSE_PER_LITER = 5880;
  static constexpr unsigned long DEFAULT_MASTER_TIMEOUT_SECONDS = 20 * 60;
  static constexpr unsigned long DEFAULT_UNDER_LIMIT_TIMEOUT_SECONDS = 10 * 60;

  SemaphoreHandle_t xSemaphore = NULL;
  TSafePreferences* systemSettingsStorage = NULL;
};
#endif