#ifndef Users_h
#define Users_h
#include <Arduino.h>
#include "TSafePreferences.h"
#include "mbedtls/md.h"

class Users {
public:
  Users();
  ~Users();
  
  static const size_t USERNAME_MAX_CHAR_COUNT = 15;
  static const size_t USERNAME_BUFFER_SIZE = USERNAME_MAX_CHAR_COUNT + 1;
  static const size_t COOKIE_BUFFER_SIZE = 199;
  static const size_t HASH_BUFFER_SIZE = 32;
  static const uint32_t PERMISSIONS_ACTIVE = 0b1;
  static const uint32_t PERMISSIONS_ADMIN = 0b10;
  static const uint32_t PERMISSIONS_PAYMENT = 0b100;

  bool createUser(const char* username, const char* displayname, const char* password, uint32_t permissions);
  bool delteUser(const char* username);
  bool verifyPassword(const char* username, const char* password);
  bool setPassword(const char* username, const char* password);
  bool getUserCookie(const char* username, char* cookie);
  bool verifyUserCookie(const char* cookie);
  int16_t getUserBill(const char* username);
  bool setUserBill(const char* username, uint16_t bill);
  bool addUserBill(const char* username, uint16_t add, uint16_t& res);
  bool checkPermissions(uint32_t permissions, uint32_t permissionMask);
  bool isPermited(const char* username, uint32_t permissionMask);
private:
  static constexpr char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7','8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
  const char* HMAC_KEY = "iR8sPr11YLDR1Ij7bw2ec70YvpJrt3gK";
  const char* NVS_PARTTION = "nvs_ext";
  const char* NAMESPACE_DISPLAY_NAMES = "usrs-dispnms";
  const char* NAMESPACE_HASHES = "usrs-hashes";
  const char* NAMESPACE_PERMISSIONS = "usrs-perms";
  const char* NAMESPACE_BILLS = "usrs-bills";
  const char* COOKIE_DELIMITER = " ";

  SemaphoreHandle_t xSemaphore = NULL;
  TSafePreferences* displayNamesStorage = NULL;
  TSafePreferences* hashesStorage = NULL;
  TSafePreferences* permissionsStorage = NULL;
  TSafePreferences* billsStorage = NULL;

  void hexStr(unsigned char *data, int len, char* buffer);
  int computeHMAChash(const char* message, unsigned char* hash);
  void composeCookieBase(const char* username, const char* displayname, uint32_t permissions, struct tm* time, char* cookieBase);
};
#endif