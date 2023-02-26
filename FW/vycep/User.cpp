#include "User.h"

User::User(SemaphoreHandle_t xSemaphore) {
  this->xSemaphore = xSemaphore;
  hashesStorage = new TSafePreferences(this->xSemaphore, NAMESPACE_HASHES, NVS_PARTTION);
  permissionsStorage = new TSafePreferences(this->xSemaphore, NAMESPACE_PERMISSIONS, NVS_PARTTION);
  billsStorage = new TSafePreferences(this->xSemaphore, NAMESPACE_BILLS, NVS_PARTTION);
  settings = new TSafePreferences(this->xSemaphore, NAMESPACE_SETTINGS, NVS_PARTTION);
}

User::~User() {
  delete (hashesStorage);
  delete (permissionsStorage);
  delete (billsStorage);
  delete (settings);
}

bool User::clearAll() {
  bool res = true;
  res = hashesStorage->clear() && res;
  res = permissionsStorage->clear() && res;
  res = billsStorage->clear() && res;
  res = settings->clear() && res;
  return res;
}

void User::getPermissionsValidityHexHash(char* username, uint32_t permissions, const unsigned char* passwordHash, char* hexHash) {
  //OTESTOVAT
  if (!Utils::toLowerStr(username)) {
    return;
  }

  char message[USERNAME_BUFFER_SIZE + INT32_CHAR_BUFFER_SIZE + HASH_HEXSTRING_BUFFER_SIZE] = { 0 };
  strcpy(message, username);
  Utils::appendChar(message, COOKIE_DELIMITER);
  utoa(permissions, &message[strlen(message)], 10);
  Utils::appendChar(message, COOKIE_DELIMITER);
  Utils::hexStr(passwordHash, HASH_BUFFER_SIZE, &message[strlen(message)]);
  unsigned char hash[HASH_BUFFER_SIZE];
  Utils::computeHmacHash(message, hash);
  Utils::hexStr(hash, HASH_BUFFER_SIZE, hexHash);
}

void User::composeCookieBase(char* username, uint32_t permissions, char* cookieBase, char* hexHash) {
  //OTESTOVAT
  struct tm timeInfo;
  Utils::actTime(timeInfo);
  composeCookieBase(username, permissions, timeInfo, cookieBase, hexHash);
}

void User::composeCookieBase(char* username, uint32_t permissions, struct tm& timeInfo, char* cookieBase, char* hexHash) {
  //OTESTOVAT
  if (!Utils::toLowerStr(username)) {
    return;
  }

  strcpy(cookieBase, username);
  Utils::appendChar(cookieBase, COOKIE_DELIMITER);
  utoa(permissions, &cookieBase[strlen(cookieBase)], 10);
  Utils::appendChar(cookieBase, COOKIE_DELIMITER);
  strftime(&cookieBase[strlen(cookieBase)], UTC_TIME_STRING_BUFFER_SIZE, COOKIE_DT_FORMAT, &timeInfo);  //2023-02-11T07:37:40
  unsigned char hash[HASH_BUFFER_SIZE];
  Utils::computeHmacHash(cookieBase, hash);
  Utils::hexStr(hash, HASH_BUFFER_SIZE, hexHash);
}

bool User::parseCookie(const char* cookie, char* username, uint32_t* permissions, struct tm* timeInfo, char* cookieHexHash, char* permissionsValidityHexHash) {
  //OTESTOVAT
  if (cookieHexHash != NULL) {
    memcpy(cookieHexHash, cookie, HASH_HEXSTRING_BUFFER_SIZE - 1);
    cookieHexHash[HASH_HEXSTRING_BUFFER_SIZE - 1] = 0;
  }

  if (permissionsValidityHexHash != NULL) {
    memcpy(permissionsValidityHexHash, &cookie[HASH_HEXSTRING_BUFFER_SIZE], HASH_HEXSTRING_BUFFER_SIZE - 1);
    permissionsValidityHexHash[HASH_HEXSTRING_BUFFER_SIZE - 1] = 0;
  }
  const char* lastStartPos = &cookie[2 * HASH_HEXSTRING_BUFFER_SIZE];
  char* pos = strchr(lastStartPos, COOKIE_DELIMITER);
  if (pos == NULL) {
    return false;
  }

  if (username != NULL) {
    memcpy(username, lastStartPos, pos - lastStartPos);
    username[pos - lastStartPos] = 0;
    if (strlen(username) == 0 || strlen(username) > USERNAME_MAX_CHAR_COUNT) {
      return false;
    }
  }

  lastStartPos = pos + 1;
  pos = strchr(lastStartPos, COOKIE_DELIMITER);
  if (pos == NULL) {
    return false;
  }

  char* pEnd = NULL;
  if (permissions != NULL) {
    *permissions = (uint32_t)strtoul(lastStartPos, &pEnd, 10);
  }

  if (pEnd == NULL || *pEnd == 0) {
    return false;
  }

  if (timeInfo != NULL && strptime(pEnd, COOKIE_DT_FORMAT, timeInfo) == NULL) {
    return false;
  }

  return true;
}

bool User::isAnyUserSet() {
  return settings->getBool(KEY_USER_IS_SET);
}

User::CredentialsVerificationResult User::validateUsername(const char* username) {
  if (username == NULL || username[0] == 0) {
    return User::CredentialsVerificationResult::USERNAME_EMPTY;
  }

  int len = Utils::strLenUTF8(username);
  if (len < USERNAME_MIN_CHAR_COUNT) {
    return User::CredentialsVerificationResult::USERNAME_SHORT;
  }

  if (len > USERNAME_MAX_CHAR_COUNT) {
    return User::CredentialsVerificationResult::USERNAME_LONG;
  }

  if (!Utils::isAlphaNumericStr(username)) {
    return User::CredentialsVerificationResult::USERNAME_INVALID_CHARACTERS;
  }

  return User::CredentialsVerificationResult::OK;
}

User::CredentialsVerificationResult User::validatePassword(const char* password) {
  if (password == NULL || password[0] == 0) {
    return User::CredentialsVerificationResult::PASSWORD_EMPTY;
  }

  int len = Utils::strLenUTF8(password);
  if (len < PASSWORD_MIN_CHAR_COUNT) {
    return User::CredentialsVerificationResult::PASSWORD_SHORT;
  }

  if (len > PASSWORD_MAX_CHAR_COUNT) {
    return User::CredentialsVerificationResult::PASSWORD_LONG;
  }

  return User::CredentialsVerificationResult::OK;
}

User::CredentialsVerificationResult User::registerUser(char* username, const char* password) {
  return createUser(username, password, 0);
}

User::CredentialsVerificationResult User::registerFisrtAdmin(char* username, const char* password) {
  if (isAnyUserSet()) {
    return User::CredentialsVerificationResult::ANY_USER_EXISTS;
  }

  return createUser(username, password, User::PERMISSIONS_ADMIN);
}

User::CredentialsVerificationResult User::createUser(char* username, const char* password, uint32_t permissions) {
  User::CredentialsVerificationResult verificationResult = validateUsername(username);
  if (verificationResult != User::CredentialsVerificationResult::OK) {
    return verificationResult;
  }

  verificationResult = validateUsername(username);
  if (verificationResult != User::CredentialsVerificationResult::OK) {
    return verificationResult;
  }

  Utils::toLowerStr(username);

  if (hashesStorage->isKey(username)) {
    return User::CredentialsVerificationResult::USERNAME_EXISTS;
  }

  unsigned char hash[HASH_BUFFER_SIZE];
  Utils::computeHmacHash(password, hash);
  int res = hashesStorage->putBytes(username, hash, HASH_BUFFER_SIZE) > 0
            && permissionsStorage->putUInt(username, permissions) > 0
            && billsStorage->putUShort(username, 0) > 0
            && settings->putBool(KEY_USER_IS_SET, true);

  if (!res) {
    return User::CredentialsVerificationResult::UNKNOWN_ERROR;
  }

  return User::CredentialsVerificationResult::OK;
}

bool User::delteUser(char* username) {
  //OTESTOVAT
  if (!Utils::toLowerStr(username)) {
    return false;
  }

  bool res = hashesStorage->remove(username) && res;
  res = permissionsStorage->remove(username) && res;
  res = billsStorage->remove(username);
  return res;
}

bool User::verifyPassword(char* username, const char* password) {
  //OTESTOVAT
  if (!Utils::toLowerStr(username)) {
    return false;
  }

  unsigned char hash1[HASH_BUFFER_SIZE];
  unsigned char hash2[HASH_BUFFER_SIZE];
  Utils::computeHmacHash(password, hash1);
  hashesStorage->getBytes(username, hash2, HASH_BUFFER_SIZE);
  return memcmp(hash1, hash2, HASH_BUFFER_SIZE) == 0;
}

bool User::setPassword(char* username, const char* password) {
  //OTESTOVAT
  if (!Utils::toLowerStr(username)) {
    return false;
  }

  unsigned char hash[HASH_BUFFER_SIZE];
  Utils::computeHmacHash(password, hash);
  return hashesStorage->putBytes(username, hash, HASH_BUFFER_SIZE);
}

bool User::getNewCookie(char* username, char* cookie) {
  //OTESTOVAT
  if (!Utils::toLowerStr(username)) {
    return false;
  }

  unsigned char passwordHash[HASH_BUFFER_SIZE];
  char cookieBase[COOKIE_BUFFER_SIZE] = { 0 };
  bool res = true;
  res = res && hashesStorage->getBytes(username, passwordHash, HASH_BUFFER_SIZE);
  if (!res) {
    return false;
  }

  uint32_t permissions = permissionsStorage->getUInt(username, 0);
  //Hash pro ověření cookie a cookie
  composeCookieBase(username, permissions, cookieBase, cookie);
  Utils::appendChar(cookie, COOKIE_DELIMITER);
  //Hash pro ověření změny hesla a práv
  getPermissionsValidityHexHash(username, permissions, passwordHash, &cookie[strlen(cookie)]);
  Utils::appendChar(cookie, COOKIE_DELIMITER);
  strcat(cookie, cookieBase);

  return true;
}

bool User::checkCookieMinimalLength(const char* cookie) {
  //OTESTOVAT
  short hashPartSize = 2 * (HASH_HEXSTRING_BUFFER_SIZE);
  if (cookie == NULL || strlen(cookie) < hashPartSize + 1) {
    return false;
  }

  return true;
}

bool User::verifyPermissionsHash(const char* cookie) {
  //OTESTOVAT
  char permissionsValidityHexHash[HASH_HEXSTRING_BUFFER_SIZE] = { 0 };
  char username[USERNAME_BUFFER_SIZE] = { 0 };

  if (!parseCookie(cookie, username, NULL, NULL, NULL, permissionsValidityHexHash)) {
    return false;
  }

  if (permissionsValidityHexHash == NULL || strlen(permissionsValidityHexHash) != HASH_HEXSTRING_BUFFER_SIZE - 1) {
    return false;
  }

  uint32_t permissions = permissionsStorage->getUInt(username, 0);
  unsigned char passwordHash[HASH_BUFFER_SIZE];
  if (!hashesStorage->getBytes(username, passwordHash, HASH_BUFFER_SIZE)) {
    return false;
  }

  char permHexHash[HASH_HEXSTRING_BUFFER_SIZE] = { 0 };
  getPermissionsValidityHexHash(username, permissions, passwordHash, permHexHash);
  return memcmp(permissionsValidityHexHash, permHexHash, HASH_HEXSTRING_BUFFER_SIZE) == 0;
}

bool User::verifyCookieHash(const char* cookie) {
  //OTESTOVAT !!!!
  if (!checkCookieMinimalLength(cookie)) {
    return false;
  }

  unsigned char hash[HASH_BUFFER_SIZE];
  Utils::computeHmacHash(&cookie[2 * (HASH_HEXSTRING_BUFFER_SIZE)], hash);
  char hexHash[HASH_HEXSTRING_BUFFER_SIZE] = { 0 };
  Utils::hexStr(hash, HASH_BUFFER_SIZE, hexHash);
  return memcmp(cookie, hexHash, 2 * HASH_BUFFER_SIZE) == 0;
}

// bool User::isAuthenticated(const char* cookie) {
//   //OTESTOVAT
//   return verifyCookieHash(cookie);
// }

// bool User::isAuthorized(const char* cookie, uint32_t permissionMask) {
//   //OTESTOVAT
//   if (!isAuthenticated(cookie)) {
//     return false;
//   }

//   if (permissionMask == 0) {
//     return true;
//   }

//   uint32_t cookiePermissions = 0;
//   if (!parseCookie(cookie, NULL, NULL, &cookiePermissions, NULL, NULL, NULL)) {
//     return false;
//   }

//   return checkPermissions(cookiePermissions, permissionMask);
// }

User::CookieVerificationResult User::getCookieInfo(const char* cookie, char* username, uint32_t* permissions, char* newCookie) {
  //OTESTOVAT
  if (!verifyCookieHash(cookie)) {
    return CookieVerificationResult::INVALID_HASH;
  }

  struct tm timeInfo;
  if (!parseCookie(cookie, username, permissions, &timeInfo, NULL, NULL)) {
    return CookieVerificationResult::INVALID_FORMAT;
  }

  time_t now;
  time(&now);
  double cookieLifeSeconds = difftime(now, mktime(&timeInfo));
  if (cookieLifeSeconds > AUTH_TIMEOUT_SEC) {
    if (!verifyPermissionsHash(cookie)) {
      return CookieVerificationResult::INVALID_PERMISSIONS;
    }

    if (newCookie != NULL && username != NULL) {
      if (getNewCookie(username, newCookie)) {
        //Vystaveno nové cookie
        return CookieVerificationResult::OUT_OF_DATE_REVALIDATED;
      }
    }

    return CookieVerificationResult::OUT_OF_DATE_UNABLE_TO_REVALIDATE;
  }

  return CookieVerificationResult::OK;
}

int16_t User::getUserBill(char* username) {
  //OTESTOVAT
  if (!Utils::toLowerStr(username)) {
    return 0;
  }

  return billsStorage->getUShort(username);
}

bool User::setUserBill(char* username, uint16_t bill) {
  //OTESTOVAT
  if (!Utils::toLowerStr(username)) {
    return false;
  }

  return billsStorage->putUShort(username, 0) > 0;
}

bool User::addUserBill(char* username, uint16_t add, uint16_t& res) {
  //OTESTOVAT
  if (!Utils::toLowerStr(username)) {
    return false;
  }

  return billsStorage->addUShort(username, add, 0, res);
}

bool User::checkPermissions(uint32_t permissions, uint32_t permissionMask) {
  //OTESTOVAT
  return permissions & permissionMask;
}

// bool User::isPermited(char* username, uint32_t permissionMask) {
//   //OTESTOVAT
//if (!Utils::toLowerStr(username)) {
//    return false;
//  }
//   return checkPermissions(permissionsStorage->getUInt(username), permissionMask);
// }