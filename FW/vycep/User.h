#ifndef User_h
#define User_h
#include <Arduino.h>
#include "TSafePreferences.h"
#include "Utils.h"

class User {
public:
  User(SemaphoreHandle_t xSemaphore);
  ~User();
  enum CookieVerificationResult {
    OK = 0,
    OUT_OF_DATE = 1,
    INVALID_HASH = 2,
    INVALID_PERMISSIONS = 3,
    INVALID_FORMAT = 4,
    UNABLE_TO_REVALIDATE = 5,
  };

  static constexpr double AUTH_TIMEOUT_SEC = 24 * 60 * 60 * 1;  //24h
  static constexpr size_t INT32_CHAR_BUFFER_SIZE = 11;
  static constexpr size_t UTC_TIME_STRING_BUFFER_SIZE = 20;
  static constexpr size_t USERNAME_MAX_CHAR_COUNT = 15;
  static constexpr size_t USERNAME_BUFFER_SIZE = USERNAME_MAX_CHAR_COUNT + 1;
  static constexpr size_t HASH_BUFFER_SIZE = 32;
  static constexpr size_t HASH_HEXSTRING_BUFFER_SIZE = 2 * HASH_BUFFER_SIZE + 1;
  static constexpr size_t COOKIE_BUFFER_SIZE = 2 * USERNAME_BUFFER_SIZE + INT32_CHAR_BUFFER_SIZE + UTC_TIME_STRING_BUFFER_SIZE + 2 * HASH_HEXSTRING_BUFFER_SIZE;
  static constexpr uint32_t PERMISSIONS_NO_PERMISSIONS = 0;
  static constexpr uint32_t PERMISSIONS_ACTIVE = 0b1;
  static constexpr uint32_t PERMISSIONS_ADMIN = 0b10;
  static constexpr uint32_t PERMISSIONS_PAYMENT = 0b100;

  bool createUser(const char* username, const char* displayname, const char* password, uint32_t permissions);
  bool delteUser(const char* username);
  bool verifyPassword(const char* username, const char* password);
  bool setPassword(const char* username, const char* password);
  bool getCookie(const char* username, char* cookie);
  bool isAuthenticated(const char* cookie);
  bool isAuthorized(const char* cookie, uint32_t permissionMask);
  CookieVerificationResult getCookieInfo(const char* cookie, char* username, uint32_t* permissions, char* newCookie);
  int16_t getUserBill(const char* username);
  bool setUserBill(const char* username, uint16_t bill);
  bool addUserBill(const char* username, uint16_t add, uint16_t& res);
  bool checkPermissions(uint32_t permissions, uint32_t permissionMask);
  bool isPermited(const char* username, uint32_t permissionMask);

private:
  static constexpr const char* NVS_PARTTION = "nvs_ext";
  static constexpr const char* NAMESPACE_DISPLAY_NAMES = "usrs-dispnms";
  static constexpr const char* NAMESPACE_HASHES = "usrs-hashes";
  static constexpr const char* NAMESPACE_PERMISSIONS = "usrs-perms";
  static constexpr const char* NAMESPACE_BILLS = "usrs-bills";
  static constexpr const char* COOKIE_DELIMITER = " ";
  static constexpr const char* COOKIE_DT_FORMAT = "%Y-%m-%dT%H:%M:%S";

  SemaphoreHandle_t xSemaphore = NULL;
  TSafePreferences* displayNamesStorage = NULL;
  TSafePreferences* hashesStorage = NULL;
  TSafePreferences* permissionsStorage = NULL;
  TSafePreferences* billsStorage = NULL;

  void getPermissionsValidityHexHash(const char* username, uint32_t permissions, const unsigned char* passwordHash, char* hexHash);
  void composeCookieBase(const char* username, const char* displayname, uint32_t permissions, char* cookieBase, char* hexHash);
  void composeCookieBase(const char* username, const char* displayname, uint32_t permissions, struct tm& timeInfo, char* cookieBase, char* hexHash);
  bool verifyPermissionsHash(const char* cookie);
  bool verifyCookieHash(const char* cookie);
  bool parseCookie(const char* cookie, char* username, char* displayname, uint32_t* permissions, struct tm* timeInfo, char* cookieHexHash, char* permissionsValidityHexHash);
  bool checkCookieMinimalLength(const char* cookie);
};
#endif