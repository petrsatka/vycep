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

void User::test() {
  if (checkPermissions(PERMISSIONS_ANY_PERMISSIONS, PERMISSIONS_ANY_PERMISSIONS)
      && checkPermissions(PERMISSIONS_ACTIVE, PERMISSIONS_ACTIVE)
      && checkPermissions(PERMISSIONS_ACTIVE | PERMISSIONS_PAYMENT, PERMISSIONS_ACTIVE | PERMISSIONS_PAYMENT)
      && checkPermissions(PERMISSIONS_ACTIVE, PERMISSIONS_ANY_PERMISSIONS)
      && checkPermissions(PERMISSIONS_INACTIVE, PERMISSIONS_ANY_PERMISSIONS)
      && checkPermissions(PERMISSIONS_ACTIVE | PERMISSIONS_ADMIN, PERMISSIONS_ADMIN)
      && !checkPermissions(PERMISSIONS_INACTIVE, PERMISSIONS_ADMIN)
      && !checkPermissions(PERMISSIONS_INACTIVE, PERMISSIONS_ACTIVE | PERMISSIONS_PAYMENT)
      && !checkPermissions(PERMISSIONS_ACTIVE, PERMISSIONS_ADMIN)) {
    sprintln("Permissions test OK");
  } else {
    sprintln("Permissions test FAILED");
  }

  getPermissionsValidityHexHash(NULL, 0, NULL, NULL);
  char hexHash[Utils::HASH_HEXSTRING_BUFFER_SIZE];
  unsigned char pwdhash[Utils::HASH_BUFFER_SIZE] = "abcertyuknfrgterasdefgrtuioplkg";
  getPermissionsValidityHexHash("abc", 123, pwdhash, hexHash);
  if (strcmp(hexHash, "7e8cf5119ae6119a16cf62046ab311c9646167cfe4cc91bd4f729f485f5696b2") == 0) {
    sprintln("getPermissionsValidityHexHash 1 test OK");
  } else {
    sprintln("getPermissionsValidityHexHash 1 test FAILED");
  }

  getPermissionsValidityHexHash("", 123, pwdhash, hexHash);
  if (strcmp(hexHash, "bf55cd839748e334db52549ec2b4c81c51cf37ba0134a46332cef6eb4365d5d3") == 0) {
    sprintln("getPermissionsValidityHexHash 2 test OK");
  } else {
    sprintln("getPermissionsValidityHexHash 2 test FAILED");
  }

  getPermissionsValidityHexHash("asdfghjklpoiuyt", 4294967295, pwdhash, hexHash);
  if (strcmp(hexHash, "66d26b0ff123d569ebb03f9b60ded79cc9438b9c7ead18200c8b8e6d02745820") == 0) {
    sprintln("getPermissionsValidityHexHash 3 test OK");
  } else {
    sprintln("getPermissionsValidityHexHash 3 test FAILED");
  }
}

bool User::clearAll() {
  dprintln("clearAll");
  bool res = true;
  res = hashesStorage->clear() && res;
  res = permissionsStorage->clear() && res;
  res = billsStorage->clear() && res;
  res = settings->clear() && res;
  return res;
}

void User::getPermissionsValidityHexHash(const char* lCaseUsername, uint32_t permissions, const unsigned char* passwordHash, char* hexHash) {
  dprintln("getPermissionsValidityHexHash");
  if (lCaseUsername == NULL || passwordHash == NULL || hexHash == NULL) {
    return;
  }

  char message[USERNAME_BUFFER_SIZE + INT32_CHAR_BUFFER_SIZE + Utils::HASH_HEXSTRING_BUFFER_SIZE] = { 0 };
  strcpy(message, lCaseUsername);
  Utils::appendChar(message, COOKIE_DELIMITER);
  utoa(permissions, &message[strlen(message)], 10);
  Utils::appendChar(message, COOKIE_DELIMITER);
  Utils::hexStr(passwordHash, Utils::HASH_BUFFER_SIZE, &message[strlen(message)]);
  unsigned char hash[Utils::HASH_BUFFER_SIZE];
  Utils::computeHmacHash(message, hash);
  Utils::hexStr(hash, Utils::HASH_BUFFER_SIZE, hexHash);
}

void User::composeCookieBase(const char* lCaseUsername, uint32_t permissions, char* cookieBase, char* hexHash) {
  sprintln("!composeCookieBase1");
  struct tm timeInfo;
  Utils::actTime(timeInfo);
  composeCookieBase(lCaseUsername, permissions, timeInfo, cookieBase, hexHash);
}

void User::composeCookieBase(const char* lCaseUsername, uint32_t permissions, struct tm& timeInfo, char* cookieBase, char* hexHash) {
  sprintln("!composeCookieBase2");
  strcpy(cookieBase, lCaseUsername);
  Utils::appendChar(cookieBase, COOKIE_DELIMITER);
  utoa(permissions, &cookieBase[strlen(cookieBase)], 10);
  Utils::appendChar(cookieBase, COOKIE_DELIMITER);
  strftime(&cookieBase[strlen(cookieBase)], UTC_TIME_STRING_BUFFER_SIZE, COOKIE_DT_FORMAT, &timeInfo);  //2023-02-11T07:37:40
  unsigned char hash[Utils::HASH_BUFFER_SIZE];
  Utils::computeHmacHash(cookieBase, hash);
  Utils::hexStr(hash, Utils::HASH_BUFFER_SIZE, hexHash);
}

bool User::parseCookie(const char* cookie, char* username, uint32_t* permissions, struct tm* timeInfo, char* cookieHexHash, char* permissionsValidityHexHash) {
  sprintln("!parseCookie");
  if (cookieHexHash != NULL) {
    memcpy(cookieHexHash, cookie, Utils::HASH_HEXSTRING_BUFFER_SIZE - 1);
    cookieHexHash[Utils::HASH_HEXSTRING_BUFFER_SIZE - 1] = 0;
  }

  if (permissionsValidityHexHash != NULL) {
    memcpy(permissionsValidityHexHash, &cookie[Utils::HASH_HEXSTRING_BUFFER_SIZE], Utils::HASH_HEXSTRING_BUFFER_SIZE - 1);
    permissionsValidityHexHash[Utils::HASH_HEXSTRING_BUFFER_SIZE - 1] = 0;
  }
  const char* lastStartPos = &cookie[2 * Utils::HASH_HEXSTRING_BUFFER_SIZE];
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
  dprintln("isAnyUserSet");
  return settings->getBool(KEY_USER_IS_SET);
}

User::CredentialsVerificationResult User::validateUsername(const char* lCaseUsername) {
  sprintln("!validateUsername");
  if (lCaseUsername == NULL || lCaseUsername[0] == 0) {
    return User::CredentialsVerificationResult::USERNAME_EMPTY;
  }

  int len = Utils::strLenUTF8(lCaseUsername);
  if (len < USERNAME_MIN_CHAR_COUNT) {
    return User::CredentialsVerificationResult::USERNAME_SHORT;
  }

  if (len > USERNAME_MAX_CHAR_COUNT) {
    return User::CredentialsVerificationResult::USERNAME_LONG;
  }

  if (!Utils::isAlphaNumericStr(lCaseUsername)) {
    return User::CredentialsVerificationResult::USERNAME_INVALID_CHARACTERS;
  }

  return User::CredentialsVerificationResult::OK;
}

User::CredentialsVerificationResult User::validatePassword(const char* password) {
  sprintln("!validatePassword");
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

User::CredentialsVerificationResult User::registerUser(const char* username, const char* password, char* lCaseUsername) {
  sprintln("!registerUser");
  return createUser(username, password, User::PERMISSIONS_INACTIVE, lCaseUsername);
}

User::CredentialsVerificationResult User::registerFirstAdmin(const char* username, const char* password, char* lCaseUsername) {
  sprintln("!registerFirstAdmin");
  if (isAnyUserSet()) {
    return User::CredentialsVerificationResult::ANY_USER_EXISTS;
  }

  return createUser(username, password, User::PERMISSIONS_ACTIVE | User::PERMISSIONS_PAYMENT | User::PERMISSIONS_ADMIN, lCaseUsername);
}

User::CredentialsVerificationResult User::createUser(const char* username, const char* password, uint32_t permissions, char* lCaseUsername) {
  sprintln("!createUser");
  User::CredentialsVerificationResult verificationResult = validateUsername(username);

  if (verificationResult != User::CredentialsVerificationResult::OK) {
    return verificationResult;
  }

  Utils::toLowerStr(username, lCaseUsername, USERNAME_BUFFER_SIZE);

  verificationResult = validatePassword(password);
  if (verificationResult != User::CredentialsVerificationResult::OK) {
    return verificationResult;
  }

  if (hashesStorage->isKey(lCaseUsername)) {
    return User::CredentialsVerificationResult::USERNAME_EXISTS;
  }

  unsigned char hash[Utils::HASH_BUFFER_SIZE];
  Utils::computeHmacHash(password, hash);
  int res = hashesStorage->putBytes(lCaseUsername, hash, Utils::HASH_BUFFER_SIZE) > 0
            && permissionsStorage->putUInt(lCaseUsername, permissions) > 0
            && billsStorage->putUShort(lCaseUsername, 0) > 0
            && settings->putBool(KEY_USER_IS_SET, true);

  if (!res) {
    return User::CredentialsVerificationResult::UNKNOWN_ERROR;
  }

  return User::CredentialsVerificationResult::OK;
}

bool User::delteUser(const char* lCaseUsername) {
  sprintln("!delteUser");
  bool res = hashesStorage->remove(lCaseUsername) && res;
  res = permissionsStorage->remove(lCaseUsername) && res;
  res = billsStorage->remove(lCaseUsername);
  return res;
}

bool User::verifyPassword(const char* lCaseUsername, const char* password) {
  sprintln("!verifyPassword");
  unsigned char hash1[Utils::HASH_BUFFER_SIZE];
  unsigned char hash2[Utils::HASH_BUFFER_SIZE];
  Utils::computeHmacHash(password, hash1);
  hashesStorage->getBytes(lCaseUsername, hash2, Utils::HASH_BUFFER_SIZE);
  return memcmp(hash1, hash2, Utils::HASH_BUFFER_SIZE) == 0;
}

bool User::setPassword(const char* lCaseUsername, const char* password) {
  sprintln("!setPassword");
  unsigned char hash[Utils::HASH_BUFFER_SIZE];
  Utils::computeHmacHash(password, hash);
  return hashesStorage->putBytes(lCaseUsername, hash, Utils::HASH_BUFFER_SIZE);
}

bool User::getNewCookie(const char* lCaseUsername, char* cookie) {
  sprintln("!getNewCookie");
  unsigned char passwordHash[Utils::HASH_BUFFER_SIZE];
  char cookieBase[COOKIE_BUFFER_SIZE] = { 0 };
  bool res = true;
  res = res && hashesStorage->getBytes(lCaseUsername, passwordHash, Utils::HASH_BUFFER_SIZE);
  if (!res) {
    return false;
  }

  uint32_t permissions = permissionsStorage->getUInt(lCaseUsername, 0);
  //Hash pro ověření cookie a cookie
  composeCookieBase(lCaseUsername, permissions, cookieBase, cookie);
  Utils::appendChar(cookie, COOKIE_DELIMITER);
  //Hash pro ověření změny hesla a práv
  getPermissionsValidityHexHash(lCaseUsername, permissions, passwordHash, &cookie[strlen(cookie)]);
  Utils::appendChar(cookie, COOKIE_DELIMITER);
  strcat(cookie, cookieBase);

  return true;
}

bool User::verifyPermissionsHash(const char* cookie) {
  sprintln("!verifyPermissionsHash");
  char permissionsValidityHexHash[Utils::HASH_HEXSTRING_BUFFER_SIZE] = { 0 };
  char username[USERNAME_BUFFER_SIZE] = { 0 };

  if (!parseCookie(cookie, username, NULL, NULL, NULL, permissionsValidityHexHash)) {
    return false;
  }

  if (permissionsValidityHexHash == NULL || strlen(permissionsValidityHexHash) != Utils::HASH_HEXSTRING_BUFFER_SIZE - 1) {
    return false;
  }

  uint32_t permissions = permissionsStorage->getUInt(username, 0);
  unsigned char passwordHash[Utils::HASH_BUFFER_SIZE];
  if (!hashesStorage->getBytes(username, passwordHash, Utils::HASH_BUFFER_SIZE)) {
    return false;
  }

  char permHexHash[Utils::HASH_HEXSTRING_BUFFER_SIZE] = { 0 };
  getPermissionsValidityHexHash(username, permissions, passwordHash, permHexHash);
  return memcmp(permissionsValidityHexHash, permHexHash, Utils::HASH_HEXSTRING_BUFFER_SIZE) == 0;
}

bool User::verifyCookieHash(const char* cookie) {
  sprintln("!verifyCookieHash");
  short propPartPos = Utils::getCookiePropsPosition(cookie);
  if (propPartPos < 0) {
    return false;
  }

  unsigned char hash[Utils::HASH_BUFFER_SIZE];
  Utils::computeHmacHash(&cookie[propPartPos], hash);
  char hexHash[Utils::HASH_HEXSTRING_BUFFER_SIZE] = { 0 };
  Utils::hexStr(hash, Utils::HASH_BUFFER_SIZE, hexHash);
  return memcmp(cookie, hexHash, 2 * Utils::HASH_BUFFER_SIZE) == 0;
}

User::CookieVerificationResult User::getCookieInfo(const char* cookie, char* username, uint32_t* permissions, char* newCookie) {
  sprintln("!getCookieInfo");
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

int16_t User::getUserBill(const char* lCaseUsername) {
  sprintln("!getUserBill");
  return billsStorage->getUShort(lCaseUsername);
}

bool User::setUserBill(const char* lCaseUsername, uint16_t bill) {
  sprintln("!setUserBill");
  return billsStorage->putUShort(lCaseUsername, 0) > 0;
}

bool User::addUserBill(const char* lCaseUsername, uint16_t add, uint16_t& res) {
  sprintln("!addUserBill");
  return billsStorage->addUShort(lCaseUsername, add, 0, res);
}

bool User::checkPermissions(uint32_t permissions, uint32_t permissionMask) {
  dprintln("checkPermissions");
  return permissions & permissionMask;
}