#ifndef Users_h
#define Users_h
#include <Arduino.h>
#include "TSafePreferences.h"

class Users {
public:
  Users();
  ~Users();
  static const size_t USERNAME_MAX_LEN = 15;
  static const size_t HASH_LEN = 256;

  bool createUser(const char* username, const char* displayname, const unsigned char* passwordHash, uint32_t permissions);
  bool delteUser(const char* username);
  bool verifyPasswordHash(const char* username, const unsigned char* passwordHash);
  bool setPasswordHash(const char* username, const unsigned char* passwordHash);
  bool getUserCookie(const char* username, char* cookie, size_t maxLen);
  bool verifyUserCookie(const char* cookie);
  int16_t getUserBill(const char* username);
  bool setUserBill(const char* username, uint16_t bill);
  bool addUserBill(const char* username, uint16_t add, uint16_t& res);
  bool isActive(uint32_t permissions);
  bool isAdmin(uint32_t permissions);
  bool canPay(uint32_t permissions);
private:
  const char* NVS_PARTTION = "nvs_ext";
  const char* NAMESPACE_DISPLAY_NAMES = "usrs-dispnms";
  const char* NAMESPACE_HASHES = "usrs-hashes";
  const char* NAMESPACE_PERMISSIONS = "usrs-perms";
  const char* NAMESPACE_BILLS = "usrs-bills";
  SemaphoreHandle_t xSemaphore = NULL;
  TSafePreferences* displayNamesStorage = NULL;
  TSafePreferences* hashesStorage = NULL;
  TSafePreferences* permissionsStorage = NULL;
  TSafePreferences* billsStorage = NULL;
};
#endif