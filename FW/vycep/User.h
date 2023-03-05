#ifndef User_h
#define User_h
#include <Arduino.h>
#include "TSafePreferences.h"
#include "Utils.h"

class User {
public:
  User(SemaphoreHandle_t xSemaphore);
  ~User();
  enum class CookieVerificationResult {
    OK = 0,
    OUT_OF_DATE_REVALIDATED = 1,
    INVALID_HASH = 2,
    INVALID_PERMISSIONS = 3,
    INVALID_FORMAT = 4,
    OUT_OF_DATE_UNABLE_TO_REVALIDATE = 5,
  };

  enum class CredentialsVerificationResult {
    OK = 0,
    USERNAME_SHORT = 1,
    USERNAME_LONG = 2,
    USERNAME_INVALID_CHARACTERS = 3,
    USERNAME_EMPTY = 4,
    PASSWORD_SHORT = 5,
    PASSWORD_LONG = 6,
    PASSWORD_EMPTY = 7,
    USERNAME_EXISTS = 8,
    UNKNOWN_ERROR = 9,
    ANY_USER_EXISTS = 10, //Otestovat
  };

  static constexpr int USERNAME_MIN_CHAR_COUNT = 5;
  static constexpr int PASSWORD_MIN_CHAR_COUNT = 6;
  static constexpr int PASSWORD_MAX_CHAR_COUNT = 30;
  static constexpr double AUTH_TIMEOUT_SEC = 24 * 60 * 60 * 1;  //24h
  static constexpr size_t INT32_CHAR_BUFFER_SIZE = 11;
  static constexpr size_t UTC_TIME_STRING_BUFFER_SIZE = 20;
  static constexpr size_t USERNAME_MAX_CHAR_COUNT = 15;
  static constexpr size_t USERNAME_BUFFER_SIZE = USERNAME_MAX_CHAR_COUNT + 1;
  static constexpr size_t HASH_BUFFER_SIZE = 32;
  static constexpr size_t HASH_HEXSTRING_BUFFER_SIZE = 2 * HASH_BUFFER_SIZE + 1;
  static constexpr size_t COOKIE_BUFFER_SIZE = USERNAME_BUFFER_SIZE + INT32_CHAR_BUFFER_SIZE + UTC_TIME_STRING_BUFFER_SIZE + 2 * HASH_HEXSTRING_BUFFER_SIZE;
  static constexpr uint32_t PERMISSIONS_ANY_PERMISSIONS = 0;
  static constexpr uint32_t PERMISSIONS_ACTIVE = 0b1;
  static constexpr uint32_t PERMISSIONS_ADMIN = 0b10;
  static constexpr uint32_t PERMISSIONS_PAYMENT = 0b100;

  //static bool isAuthenticated(const char* cookie);
  //static bool isAuthorized(const char* cookie, uint32_t permissionMask);
  static bool checkPermissions(uint32_t permissions, uint32_t permissionMask);

  bool isAnyUserSet();
  bool delteUser(const char* lCaseUsername);
  bool verifyPassword(const char* lCaseUsername, const char* password);
  bool setPassword(const char* lCaseUsername, const char* password);
  bool getNewCookie(const char* lCaseUsername, char* cookie);
  CookieVerificationResult getCookieInfo(const char* cookie, char* username, uint32_t* permissions, char* newCookie);
  int16_t getUserBill(const char* lCaseUsername);
  bool setUserBill(const char* lCaseUsername, uint16_t bill);
  bool addUserBill(const char* lCaseUsername, uint16_t add, uint16_t& res);
  User::CredentialsVerificationResult registerUser(const char* username, const char* password, char* lCaseUsername);
  User::CredentialsVerificationResult registerFirstAdmin(const char* username, const char* password, char* lCaseUsername);
  //bool isPermited(const char* lCaseUsername, uint32_t permissionMask);
  bool clearAll();

private:
  static constexpr const char* NVS_PARTTION = "nvs_ext";
  static constexpr const char* NAMESPACE_HASHES = "usrs-hashes";
  static constexpr const char* NAMESPACE_PERMISSIONS = "usrs-perms";
  static constexpr const char* NAMESPACE_BILLS = "usrs-bills";
  static constexpr const char* NAMESPACE_SETTINGS = "usrs-sets";
  static constexpr const char* COOKIE_DT_FORMAT = "%Y-%m-%dT%H:%M:%S";
  static constexpr const char* KEY_USER_IS_SET = "usr-is-set";
  static constexpr const char COOKIE_DELIMITER = ' ';

  SemaphoreHandle_t xSemaphore = NULL;
  TSafePreferences* hashesStorage = NULL;
  TSafePreferences* permissionsStorage = NULL;
  TSafePreferences* billsStorage = NULL;
  TSafePreferences* settings = NULL;

  static bool verifyCookieHash(const char* cookie);
  static bool checkCookieMinimalLength(const char* cookie);
  static bool parseCookie(const char* cookie, char* username, uint32_t* permissions, struct tm* timeInfo, char* cookieHexHash, char* permissionsValidityHexHash);
  static void composeCookieBase(const char* lCaseUsername, uint32_t permissions, char* cookieBase, char* hexHash);
  static void composeCookieBase(const char* lCaseUsername, uint32_t permissions, struct tm& timeInfo, char* cookieBase, char* hexHash);
  static CredentialsVerificationResult validateUsername(const char* lCaseUsername);
  static CredentialsVerificationResult validatePassword(const char* password);

  User::CredentialsVerificationResult createUser(const char* username, const char* password, uint32_t permissions, char* lCaseUsername);
  void getPermissionsValidityHexHash(const char* lCaseUsername, uint32_t permissions, const unsigned char* passwordHash, char* hexHash);
  bool verifyPermissionsHash(const char* cookie);
};
#endif