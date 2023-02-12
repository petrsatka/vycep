#ifndef User_h
#define User_h
#include <Arduino.h>
#include "TSafePreferences.h"
#include "mbedtls/md.h"

class User {
public:
  User();
  ~User();

  static constexpr size_t INT32_CHAR_BUFFER_SIZE = 11;
  static constexpr size_t UTC_TIME_STRING_BUFFER_SIZE = 20;
  static constexpr size_t USERNAME_MAX_CHAR_COUNT = 15;
  static constexpr size_t USERNAME_BUFFER_SIZE = USERNAME_MAX_CHAR_COUNT + 1;
  static constexpr size_t HASH_BUFFER_SIZE = 32;
  static constexpr size_t COOKIE_BUFFER_SIZE = 2 * USERNAME_BUFFER_SIZE + INT32_CHAR_BUFFER_SIZE + UTC_TIME_STRING_BUFFER_SIZE + 2 * (2 * HASH_BUFFER_SIZE + 1);
  static constexpr uint32_t PERMISSIONS_ACTIVE = 0b1;
  static constexpr uint32_t PERMISSIONS_ADMIN = 0b10;
  static constexpr uint32_t PERMISSIONS_PAYMENT = 0b100;

  bool createUser(const char* username, const char* displayname, const char* password, uint32_t permissions);
  bool delteUser(const char* username);
  bool verifyPassword(const char* username, const char* password);
  bool setPassword(const char* username, const char* password);
  bool getCookie(const char* username, char* cookie);
  bool verifyCookie(const char* cookie);
  int16_t getUserBill(const char* username);
  bool setUserBill(const char* username, uint16_t bill);
  bool addUserBill(const char* username, uint16_t add, uint16_t& res);
  bool checkPermissions(uint32_t permissions, uint32_t permissionMask);
  bool isPermited(const char* username, uint32_t permissionMask);

  //private:
  static constexpr const char* hexmap = "0123456789abcdef";
  static constexpr const char* HMAC_KEY = "iR8sPr11YLDR1Ij7bw2ec70YvpJrt3gK";
  static constexpr const char* NVS_PARTTION = "nvs_ext";
  static constexpr const char* NAMESPACE_DISPLAY_NAMES = "usrs-dispnms";
  static constexpr const char* NAMESPACE_HASHES = "usrs-hashes";
  static constexpr const char* NAMESPACE_PERMISSIONS = "usrs-perms";
  static constexpr const char* NAMESPACE_BILLS = "usrs-bills";
  static constexpr const char* COOKIE_DELIMITER = " ";

  SemaphoreHandle_t xSemaphore = NULL;
  TSafePreferences* displayNamesStorage = NULL;
  TSafePreferences* hashesStorage = NULL;
  TSafePreferences* permissionsStorage = NULL;
  TSafePreferences* billsStorage = NULL;

  void actTime(struct tm &timeInfo);
  void hexStr(const unsigned char* data, int len, char* buffer);
  int computeHmacHash(const char* message, unsigned char* hash);
  void getPermissionsValidityHash(const char* username, uint32_t permissions, const unsigned char* passwordHash, char* hexHash);
  void composeCookieBase(const char* username, const char* displayname, uint32_t permissions, char* cookieBase, char* hexHash);
  void composeCookieBase(const char* username, const char* displayname, uint32_t permissions, struct tm &timeInfo, char* cookieBase, char* hexHash);
};
#endif