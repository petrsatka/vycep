//Správa uživatelů
#ifndef User_h
#define User_h
#include <Arduino.h>
#include "TSafePreferences.h"
#include "Utils.h"
#include "Debug.h"

class User {
public:
  User(SemaphoreHandle_t xSemaphore, const char* nvsParttionName);
  ~User();

  //Výsledek ověření cookie
  enum class CookieVerificationResult {
    OK = 0,
    OUT_OF_DATE_REVALIDATED = 1,  //Cookie mimo platnost, nezměnilo se oprávnění ani heslo, úspěšně vystaveno nové cookie. Není nutné nové přihlášení
    INVALID_HASH = 2,
    INVALID_PERMISSIONS = 3,  //Změnilo se oprávnění, nebo heslo. Je nutné nové přihlášení
    INVALID_FORMAT = 4,
    OUT_OF_DATE_UNABLE_TO_REVALIDATE = 5,  //Cookie mimo platnost, nezměnilo se oprávnění ani heslo, nové cookie nebylo možné vystavit
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
    ANY_USER_EXISTS = 10,
    USERNAME_NOT_EXISTS = 11,
    INVALID_PASSWORD = 12,
  };

  static constexpr int USERNAME_MIN_CHAR_COUNT = 5;
  static constexpr int PASSWORD_MIN_CHAR_COUNT = 6;
  static constexpr int PASSWORD_MAX_CHAR_COUNT = 30;
  static constexpr double AUTH_TIMEOUT_SEC = 1 * 60 * 60 * 1;  //1h
  static constexpr size_t INT32_CHAR_BUFFER_SIZE = 11;
  static constexpr size_t UTC_TIME_STRING_BUFFER_SIZE = 20;
  static constexpr size_t USERNAME_MAX_CHAR_COUNT = 15;
  static constexpr size_t USERNAME_BUFFER_SIZE = USERNAME_MAX_CHAR_COUNT + 1;
  static constexpr size_t COOKIE_BUFFER_SIZE = USERNAME_BUFFER_SIZE + INT32_CHAR_BUFFER_SIZE + UTC_TIME_STRING_BUFFER_SIZE + 2 * Utils::HASH_HEXSTRING_BUFFER_SIZE;
  static constexpr uint32_t PERMISSIONS_ANY_PERMISSIONS = -1;
  static constexpr uint32_t PERMISSIONS_INACTIVE = 0b1;
  static constexpr uint32_t PERMISSIONS_ACTIVE = 0b10;
  static constexpr uint32_t PERMISSIONS_ADMIN = 0b100;
  static constexpr uint32_t PERMISSIONS_PAYMENT = 0b1000;

  static bool checkPermissions(uint32_t permissions, uint32_t permissionMask);

  uint32_t getPermissions(const char* lCaseUsername);
  bool isAnyUserSet();
  bool isUserSet(const char* lCaseUsername);
  void iterateUsers(KeyIterationCallback iterationCallback);
  unsigned short getUserCount();
  bool deleteUser(const char* lCaseUsername);
  User::CredentialsVerificationResult verifyPassword(const char* username, const char* password, char* lCaseUsername);
  User::CredentialsVerificationResult setPassword(const char* lCaseUsername, const char* password);
  User::CredentialsVerificationResult changePassword(const char* username, const char* oldPassword, const char* newPassword);
  bool getNewCookie(const char* lCaseUsername, char* cookie);
  CookieVerificationResult getCookieInfo(const char* cookie, bool revalidateCookie, char* username, uint32_t* permissions, char* newCookie);  //Získá infromace o cookie. Případně vypršeného ccokie, revaliduje a v případě úspěchu vytvoří nové cookie.
  int16_t getUserBill(const char* lCaseUsername);
  bool setUserBill(const char* lCaseUsername, uint16_t bill);
  bool addUserBill(const char* lCaseUsername, uint16_t add, uint16_t& res);
  User::CredentialsVerificationResult registerUser(const char* username, const char* password, char* lCaseUsername);  //Očekává jméno, heslo. Vrací result a lower case jméno.
  User::CredentialsVerificationResult registerFirstAdmin(const char* username, const char* password, char* lCaseUsername);
  bool clearAll();

  void test();

private:
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
  static bool parseCookie(const char* cookie, char* username, uint32_t* permissions, struct tm* timeInfo, char* cookieHexHash, char* permissionsValidityHexHash);
  static bool composeCookieBase(const char* lCaseUsername, uint32_t permissions, char* cookieBase, char* hexHash);                       //Volá composeCookieBase s aktuálním časem
  static bool composeCookieBase(const char* lCaseUsername, uint32_t permissions, struct tm& timeInfo, char* cookieBase, char* hexHash);  //Sestaví obsah cookie a vrátí obsah a hash obsahu
  static CredentialsVerificationResult validateUsername(const char* lCaseUsername);
  static CredentialsVerificationResult validatePassword(const char* password);

  User::CredentialsVerificationResult createUser(const char* username, const char* password, uint32_t permissions, char* lCaseUsername);
  void getPermissionsValidityHexHash(const char* lCaseUsername, uint32_t permissions, const unsigned char* passwordHash, char* hexHash);  //Vrací hash pro ověření změny hesla a oprávnění
  bool verifyPermissionsHash(const char* cookie);
};
#endif