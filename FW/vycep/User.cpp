#include "User.h"

User::User(SemaphoreHandle_t xSemaphore, const char* nvsParttionName) {
  this->xSemaphore = xSemaphore;
  hashesStorage = new TSafePreferences(this->xSemaphore, NAMESPACE_HASHES, nvsParttionName);
  permissionsStorage = new TSafePreferences(this->xSemaphore, NAMESPACE_PERMISSIONS, nvsParttionName);
  billsStorage = new TSafePreferences(this->xSemaphore, NAMESPACE_BILLS, nvsParttionName);
  settings = new TSafePreferences(this->xSemaphore, NAMESPACE_SETTINGS, nvsParttionName);
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
    //sprintln("Permissions test OK");
  } else {
    sprintln("Permissions test FAILED");
  }

  getPermissionsValidityHexHash(NULL, 0, NULL, NULL);
  char hexHash[Utils::HASH_HEXSTRING_BUFFER_SIZE];
  unsigned char pwdhash[Utils::HASH_BUFFER_SIZE] = "abcertyuknfrgterasdefgrtuioplkg";
  getPermissionsValidityHexHash("abc", 123, pwdhash, hexHash);
  if (strcmp(hexHash, "7e8cf5119ae6119a16cf62046ab311c9646167cfe4cc91bd4f729f485f5696b2") == 0) {
    //sprintln("getPermissionsValidityHexHash 1 test OK");
  } else {
    sprintln("getPermissionsValidityHexHash 1 test FAILED");
  }

  getPermissionsValidityHexHash("", 123, pwdhash, hexHash);
  if (strcmp(hexHash, "bf55cd839748e334db52549ec2b4c81c51cf37ba0134a46332cef6eb4365d5d3") == 0) {
    //sprintln("getPermissionsValidityHexHash 2 test OK");
  } else {
    sprintln("getPermissionsValidityHexHash 2 test FAILED");
  }

  getPermissionsValidityHexHash("asdfghjklpoiuyt", 4294967295, pwdhash, hexHash);
  if (strcmp(hexHash, "66d26b0ff123d569ebb03f9b60ded79cc9438b9c7ead18200c8b8e6d02745820") == 0) {
    //sprintln("getPermissionsValidityHexHash 3 test OK");
  } else {
    sprintln("getPermissionsValidityHexHash 3 test FAILED");
  }

  if (validatePassword(NULL) == User::CredentialsVerificationResult::PASSWORD_EMPTY
      && validatePassword("") == User::CredentialsVerificationResult::PASSWORD_EMPTY
      && validatePassword("ččččč") == User::CredentialsVerificationResult::PASSWORD_SHORT
      && validatePassword("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa") == User::CredentialsVerificationResult::PASSWORD_LONG
      && validatePassword("šššššššššššššššššššššššššššššš") == User::CredentialsVerificationResult::OK) {
    //sprintln("validatePassword test OK");
  } else {
    sprintln("validatePassword test FAILED");
  }

  if (validateUsername(NULL) == User::CredentialsVerificationResult::USERNAME_EMPTY
      && validateUsername("") == User::CredentialsVerificationResult::USERNAME_EMPTY
      && validateUsername("aaaa") == User::CredentialsVerificationResult::USERNAME_SHORT
      && validateUsername("aaaaaaaaaaaaaaaa") == User::CredentialsVerificationResult::USERNAME_LONG
      && validateUsername("aaaaaaš") == User::CredentialsVerificationResult::USERNAME_INVALID_CHARACTERS
      && validateUsername("aaaaaa ") == User::CredentialsVerificationResult::USERNAME_INVALID_CHARACTERS
      && validateUsername("aaaaaa.") == User::CredentialsVerificationResult::USERNAME_INVALID_CHARACTERS
      && validateUsername("12345qwertzu789") == User::CredentialsVerificationResult::OK) {
    //sprintln("validateUsername test OK");
  } else {
    sprintln("validateUsername test FAILED");
  }

  char cookie[COOKIE_BUFFER_SIZE] = { 0 };
  if (!composeCookieBase(NULL, 4294967295, NULL, NULL)
      && !composeCookieBase("aaaaaaaaaaaaaaaa", 4294967295, cookie, hexHash)
      && composeCookieBase("aaaaaaaaaaaaaaa", 4294967295, cookie, hexHash)
      && composeCookieBase("", 4294967295, cookie, hexHash)) {
    //sprintln("composeCookieBase test OK");
  } else {
    sprintln("composeCookieBase test FAILED");
  }

  if (!getNewCookie(NULL, NULL)
      && !getNewCookie("aaaaaaaaaaaaaaaa", cookie)
      && !getNewCookie("aaaaaaaaaaaaaaa", cookie)
      && !getNewCookie("", cookie)) {
    //sprintln("getNewCookie test OK");
  } else {
    sprintln("getNewCookie test FAILED");
  }

  char lCaseUsername[USERNAME_BUFFER_SIZE];
  if (registerFirstAdmin(NULL, "aaaaaaaaaaaaaaab", lCaseUsername) != User::CredentialsVerificationResult::OK
      && registerFirstAdmin("", "aaaaaaaaaaaaaaab", lCaseUsername) != User::CredentialsVerificationResult::OK
      && registerFirstAdmin("aaaaaaaaab", NULL, lCaseUsername) != User::CredentialsVerificationResult::OK
      && registerFirstAdmin("aaaaaaaaab", "", lCaseUsername) != User::CredentialsVerificationResult::OK
      && registerFirstAdmin("aaaaaaaaab", "bbbbbbbbbc", NULL) != User::CredentialsVerificationResult::OK
      && registerFirstAdmin("aab", "bbbbbbbbbc", lCaseUsername) != User::CredentialsVerificationResult::OK
      && registerFirstAdmin("aaaaaaaaaaaaaaab", "bbbbbbbbbc", lCaseUsername) != User::CredentialsVerificationResult::OK
      && registerFirstAdmin("aaaaaaaaaaaaaab", "bbbbc", lCaseUsername) != User::CredentialsVerificationResult::OK
      && registerFirstAdmin("aaaaaaaaaaaaaab", "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbc", lCaseUsername) != User::CredentialsVerificationResult::OK
      && registerFirstAdmin("aaaaaaaaaaaaaab", "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbc", lCaseUsername) == User::CredentialsVerificationResult::OK
      && registerFirstAdmin("aaaaaaaaaaaaaac", "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbc", lCaseUsername) != User::CredentialsVerificationResult::OK) {
    //sprintln("registerFirstAdmin test OK");
  } else {
    sprintln("registerFirstAdmin test FAILED");
  }

  if (createUser(NULL, "aaaaaaaaaaaaaaaa", 4294967295, lCaseUsername) != User::CredentialsVerificationResult::OK
      && createUser("", "aaaaaaaaaaaaaaaa", 4294967295, lCaseUsername) != User::CredentialsVerificationResult::OK
      && createUser("aaaaaaaaaa", NULL, 4294967295, lCaseUsername) != User::CredentialsVerificationResult::OK
      && createUser("aaaaaaaaaa", "", 4294967295, lCaseUsername) != User::CredentialsVerificationResult::OK
      && createUser("aaaaaaaaaa", "bbbbbbbbbb", 4294967295, NULL) != User::CredentialsVerificationResult::OK
      && createUser("aaa", "bbbbbbbbbb", 4294967295, lCaseUsername) != User::CredentialsVerificationResult::OK
      && createUser("aaaaaaaaaaaaaaaa", "bbbbbbbbbb", 4294967295, lCaseUsername) != User::CredentialsVerificationResult::OK
      && createUser("aaaaaaaaaaaaaaa", "bbbbb", 4294967295, lCaseUsername) != User::CredentialsVerificationResult::OK
      && createUser("aaaaaaaaaaaaaaa", "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb", 4294967295, lCaseUsername) != User::CredentialsVerificationResult::OK
      && createUser("aaaaaaaaaaaaaaa", "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbb", 4294967295, lCaseUsername) == User::CredentialsVerificationResult::OK
      && createUser("aaaaaaaaaaaaaaa", "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbb", 4294967295, lCaseUsername) != User::CredentialsVerificationResult::OK) {
    //sprintln("createUser test OK");
  } else {
    sprintln("createUser test FAILED");
  }

  char username[USERNAME_BUFFER_SIZE];
  uint32_t permissions = 0;
  struct tm timeInfo;
  char cookieHexHash[Utils::HASH_HEXSTRING_BUFFER_SIZE];
  char permissionsHash[Utils::HASH_HEXSTRING_BUFFER_SIZE];

  if (!parseCookie(NULL, username, &permissions, &timeInfo, cookieHexHash, permissionsHash)
      && !parseCookie("", username, &permissions, &timeInfo, cookieHexHash, permissionsHash)
      && !parseCookie("ba59c6e6760031958a3a598b3afc772a91ebda9f0de4344bbbad314eb65eeabc e7623fbae5eabb2c891a5574d444fbfce3c8e9e51080f634b13443675cbad170 brumbal 14 ", username, &permissions, &timeInfo, cookieHexHash, permissionsHash)
      && !parseCookie("ba59c6e6760031958a3a598b3afc772a91ebda9f0de4344bbbad314eb65eeabc e7623fbae5eabb2c891a5574d444fbfce3c8e9e51080f634b13443675cbad170 brumbal", username, &permissions, &timeInfo, cookieHexHash, permissionsHash)
      && !parseCookie("ba59c6e6760031958a3a598b3afc772a91ebda9f0de4344bbbad314eb65eeabc e7623fbae5eabb2c891a5574d444fbfce3c8e9e51080f634b13443675cbad170", username, &permissions, &timeInfo, cookieHexHash, permissionsHash)
      && !parseCookie("ba59c6e6760031958a3a598b3afc772a91ebda9f0de4344bbbad314eb65eeabc e7623fbae5eabb2c891a5574d444fbfce3c8e9e51080f634b13443675cbad17", username, &permissions, &timeInfo, cookieHexHash, permissionsHash)
      && parseCookie("ba59c6e6760031958a3a598b3afc772a91ebda9f0de4344bbbad314eb65eeabc e7623fbae5eabb2c891a5574d444fbfce3c8e9e51080f634b13443675cbad170 brumbal 14 2023-03-12T20:50:15", NULL, NULL, NULL, NULL, NULL)
      && !parseCookie("ba59c6e6760031958a3a598b3afc772a91ebda9f0de4344bbbad314eb65eeabc e7623fbae5eabb2c891a5574d444fbfce3c8e9e51080f634b13443675cbad170 brumbalxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 14 2023-03-12T20:50:15", username, &permissions, &timeInfo, cookieHexHash, permissionsHash)
      && parseCookie("ba59c6e6760031958a3a598b3afc772a91ebda9f0de4344bbbad314eb65eeabc e7623fbae5eabb2c891a5574d444fbfce3c8e9e51080f634b13443675cbad170 brumbal 14 2023-03-12T20:50:15", username, &permissions, &timeInfo, cookieHexHash, permissionsHash)
      && strcmp(username, "brumbal") == 0 && permissions == 14 && strcmp(cookieHexHash, "ba59c6e6760031958a3a598b3afc772a91ebda9f0de4344bbbad314eb65eeabc") == 0 && strcmp(permissionsHash, "e7623fbae5eabb2c891a5574d444fbfce3c8e9e51080f634b13443675cbad170") == 0
      && timeInfo.tm_year == 2023 - 1900 && timeInfo.tm_mon == 2 && timeInfo.tm_mday == 12 && timeInfo.tm_hour == 20 && timeInfo.tm_min == 50 && timeInfo.tm_sec == 15) {
    //sprintln("parseCookie test OK");
  } else {
    sprintln("parseCookie test FAILED");
  }

  if (!verifyCookieHash(NULL)
      && !verifyCookieHash("")
      && !verifyCookieHash("ba59c6e6760031958a3a598b3afc772a91ebda9f0de4344bbbad314eb65eeabc e7623fbae5eabb2c891a5574d444fbfce3c8e9e51080f634b13443675cbad170 brumbal 14 2023-03-12T20:50:16")
      && !verifyCookieHash("ba59c6e6760031958a3a598b3afc772a91ebda9f0de4344bbbad314eb65eeabc e7623fbae5eabb2c891a5574d444fbfce3c8e9e51080f634b13443675cbad170 brumbal 13 2023-03-12T20:50:15")
      && !verifyCookieHash("ba59c6e6760031958a3a598b3afc772a91ebda9f0de4344bbbad314eb65eeabc e7623fbae5eabb2c891a5574d444fbfce3c8e9e51080f634b13443675cbad170 brumbax 14 2023-03-12T20:50:15")
      && verifyCookieHash("ba59c6e6760031958a3a598b3afc772a91ebda9f0de4344bbbad314eb65eeabc e7623fbae5eabb2c891a5574d444fbfce3c8e9e51080f634b13443675cbad170 brumbal 14 2023-03-12T20:50:15")) {
    //sprintln("verifyCookieHash test OK");
  } else {
    sprintln("verifyCookieHash test FAILED");
  }

  // createUser("Brumbal", "Brumbal", PERMISSIONS_ACTIVE | PERMISSIONS_PAYMENT | PERMISSIONS_ADMIN, lCaseUsername);
  // char newCookie[COOKIE_BUFFER_SIZE];
  // if (getCookieInfo(NULL, username, &permissions, newCookie) >= User::CookieVerificationResult::INVALID_HASH
  //     && getCookieInfo("ba59c6e6760031958a3a598b3afc772a91ebda9f0de4344bbbad314eb65eeabc e7623fbae5eabb2c891a5574d444fbfce3c8e9e51080f634b13443675cbad170 brumbal 14 2023-03-12T20:50:15", NULL, &permissions, newCookie) == User::CookieVerificationResult::OK
  //     && getCookieInfo("ba59c6e6760031958a3a598b3afc772a91ebda9f0de4344bbbad314eb65eeabc e7623fbae5eabb2c891a5574d444fbfce3c8e9e51080f634b13443675cbad170 brumbal 14 2023-03-12T20:50:15", username, NULL, newCookie) == User::CookieVerificationResult::OK
  //     && getCookieInfo("7ba0f007c4a82291d35e594d639ef9673928c3d985da9381c85099620bda1a12 5db3f56bbe4a48790c561c06923fb553596c456e36deff257fedde237262feb3 halohalo 14 1970-01-01T00:00:23", username, &permissions, newCookie) == User::CookieVerificationResult::INVALID_PERMISSIONS
  //     && getCookieInfo("275e72ec7b5b3fc46e277ce29610106065458fcdf8386781653e0498dc88eba3 e7623fbae5eabb2c891a5574d444fbfce3c8e9e51080f634b13443675cbad170 brumbal 14 1970-01-01T00:00:20", username, &permissions, NULL) == User::CookieVerificationResult::OUT_OF_DATE_UNABLE_TO_REVALIDATE
  //     && getCookieInfo("275e72ec7b5b3fc46e277ce29610106065458fcdf8386781653e0498dc88eba3 e7623fbae5eabb2c891a5574d444fbfce3c8e9e51080f634b13443675cbad170 brumbal 14 1970-01-01T00:00:20", username, &permissions, newCookie) == User::CookieVerificationResult::OUT_OF_DATE_REVALIDATED) {
  //   //sprintln("getCookieInfo test OK");
  //   //sprintln(newCookie);
  // } else {
  //   sprintln("getCookieInfo test FAILED");
  // }
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

bool User::composeCookieBase(const char* lCaseUsername, uint32_t permissions, char* cookieBase, char* hexHash) {
  dprintln("composeCookieBase1");
  struct tm timeInfo;
  Utils::actTime(timeInfo);
  return composeCookieBase(lCaseUsername, permissions, timeInfo, cookieBase, hexHash);
}

bool User::composeCookieBase(const char* lCaseUsername, uint32_t permissions, struct tm& timeInfo, char* cookieBase, char* hexHash) {
  dprintln("composeCookieBase2");
  if (lCaseUsername == NULL || strlen(lCaseUsername) > USERNAME_MAX_CHAR_COUNT || cookieBase == NULL || hexHash == NULL) {
    return false;
  }

  strcpy(cookieBase, lCaseUsername);
  Utils::appendChar(cookieBase, COOKIE_DELIMITER);
  utoa(permissions, &cookieBase[strlen(cookieBase)], 10);
  Utils::appendChar(cookieBase, COOKIE_DELIMITER);
  strftime(&cookieBase[strlen(cookieBase)], UTC_TIME_STRING_BUFFER_SIZE, COOKIE_DT_FORMAT, &timeInfo);  //2023-02-11T07:37:40
  unsigned char hash[Utils::HASH_BUFFER_SIZE];
  Utils::computeHmacHash(cookieBase, hash);
  Utils::hexStr(hash, Utils::HASH_BUFFER_SIZE, hexHash);
  return true;
}

bool User::parseCookie(const char* cookie, char* username, uint32_t* permissions, struct tm* timeInfo, char* cookieHexHash, char* permissionsValidityHexHash) {
  dprintln("parseCookie");
  if (cookie == NULL || cookie[0] == 0 || Utils::getCookiePropsPosition(cookie) < 0) {
    return false;
  }

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
    short len = pos - lastStartPos;
    if (len > USERNAME_MAX_CHAR_COUNT) {
      return false;
    }
    memcpy(username, lastStartPos, len);
    username[len] = 0;
    if (username[0] == 0) {
      return false;
    }
  }

  lastStartPos = pos + 1;
  pos = strchr(lastStartPos, COOKIE_DELIMITER);
  if (pos == NULL) {
    return false;
  }

  char* pEnd = NULL;
  uint32_t perm = (uint32_t)strtoul(lastStartPos, &pEnd, 10);
  if (permissions != NULL) {
    *permissions = perm;
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
bool User::isUserSet(const char* lCaseUsername) {
  return hashesStorage->isKey(lCaseUsername);
}

User::CredentialsVerificationResult User::validateUsername(const char* lCaseUsername) {
  dprintln("validateUsername");
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
  dprintln("validatePassword");
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
  dprintln("registerFirstAdmin");
  if (isAnyUserSet()) {
    return User::CredentialsVerificationResult::ANY_USER_EXISTS;
  }

  return createUser(username, password, User::PERMISSIONS_ACTIVE | User::PERMISSIONS_PAYMENT | User::PERMISSIONS_ADMIN, lCaseUsername);
}

User::CredentialsVerificationResult User::createUser(const char* username, const char* password, uint32_t permissions, char* lCaseUsername) {
  dprintln("createUser");
  if (lCaseUsername == NULL) {
    return User::CredentialsVerificationResult::UNKNOWN_ERROR;
  }

  User::CredentialsVerificationResult verificationResult = validateUsername(username);

  if (verificationResult != User::CredentialsVerificationResult::OK) {
    return verificationResult;
  }

  if (!Utils::toLowerStr(username, lCaseUsername, USERNAME_BUFFER_SIZE)) {
    return User::CredentialsVerificationResult::UNKNOWN_ERROR;
  };

  verificationResult = validatePassword(password);
  if (verificationResult != User::CredentialsVerificationResult::OK) {
    return verificationResult;
  }

  if (isUserSet(lCaseUsername)) {
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

User::CredentialsVerificationResult User::verifyPassword(const char* username, const char* password, char* lCaseUsername) {
  sprintln("!verifyPassword");

  if (!Utils::toLowerStr(username, lCaseUsername, USERNAME_BUFFER_SIZE)) {
    return User::CredentialsVerificationResult::UNKNOWN_ERROR;
  };

  unsigned char hash1[Utils::HASH_BUFFER_SIZE];
  unsigned char hash2[Utils::HASH_BUFFER_SIZE];
  Utils::computeHmacHash(password, hash1);
  if (hashesStorage->getBytes(lCaseUsername, hash2, Utils::HASH_BUFFER_SIZE) == 0) {
    return User::CredentialsVerificationResult::USERNAME_NOT_EXISTS;
  }

  if (memcmp(hash1, hash2, Utils::HASH_BUFFER_SIZE) != 0) {
    return User::CredentialsVerificationResult::INVALID_PASSWORD;
  }

  return User::CredentialsVerificationResult::OK;
}

User::CredentialsVerificationResult User::setPassword(const char* lCaseUsername, const char* password) {
  sprintln("!setPassword");
  if (!isUserSet(lCaseUsername)) {
    return User::CredentialsVerificationResult::USERNAME_NOT_EXISTS;
  }

  User::CredentialsVerificationResult res = validatePassword(password);
  if (res == User::CredentialsVerificationResult::OK) {
    unsigned char hash[Utils::HASH_BUFFER_SIZE];
    Utils::computeHmacHash(password, hash);
    if (hashesStorage->putBytes(lCaseUsername, hash, Utils::HASH_BUFFER_SIZE) != 0) {
      res = User::CredentialsVerificationResult::OK;
    } else {
      res = User::CredentialsVerificationResult::UNKNOWN_ERROR;
    }
  }

  return res;
}

User::CredentialsVerificationResult User::changePassword(const char* username, const char*oldPassword, const char*newPassword) {
  sprintln("!changePassword");
  char lCaseUsername[User::USERNAME_BUFFER_SIZE] = { 0 };
  User::CredentialsVerificationResult res = verifyPassword(username, oldPassword, lCaseUsername);
  if (res == User::CredentialsVerificationResult::OK) {
    res = setPassword(lCaseUsername, newPassword);
  }

  return res;
}

bool User::getNewCookie(const char* lCaseUsername, char* cookie) {
  dprintln("getNewCookie");
  if (lCaseUsername == NULL || lCaseUsername[0] == 0 || cookie == NULL) {
    return false;
  }

  unsigned char passwordHash[Utils::HASH_BUFFER_SIZE];
  char cookieBase[COOKIE_BUFFER_SIZE] = { 0 };
  bool res = true;
  res = res && hashesStorage->getBytes(lCaseUsername, passwordHash, Utils::HASH_BUFFER_SIZE);
  if (!res) {
    return false;
  }

  uint32_t permissions = permissionsStorage->getUInt(lCaseUsername, 0);
  //Hash pro ověření cookie a cookie
  if (!composeCookieBase(lCaseUsername, permissions, cookieBase, cookie)) {
    return false;
  }

  Utils::appendChar(cookie, COOKIE_DELIMITER);
  //Hash pro ověření změny hesla a práv
  getPermissionsValidityHexHash(lCaseUsername, permissions, passwordHash, &cookie[strlen(cookie)]);
  Utils::appendChar(cookie, COOKIE_DELIMITER);
  strcat(cookie, cookieBase);

  return true;
}

bool User::verifyPermissionsHash(const char* cookie) {
  dprintln("verifyPermissionsHash");
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
  dprintln("verifyCookieHash");
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
  dprintln("getCookieInfo");
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