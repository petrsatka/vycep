#include "User.h"

User::User() {
  xSemaphore = xSemaphoreCreateMutex();
  displayNamesStorage = new TSafePreferences(this->xSemaphore, NAMESPACE_DISPLAY_NAMES, NVS_PARTTION);
  hashesStorage = new TSafePreferences(this->xSemaphore, NAMESPACE_HASHES, NVS_PARTTION);
  permissionsStorage = new TSafePreferences(this->xSemaphore, NAMESPACE_PERMISSIONS, NVS_PARTTION);
  billsStorage = new TSafePreferences(this->xSemaphore, NAMESPACE_BILLS, NVS_PARTTION);
}

User::~User() {
  delete (displayNamesStorage);
  delete (hashesStorage);
  delete (permissionsStorage);
  delete (billsStorage);
  vSemaphoreDelete(xSemaphore);
}

void User::actTime(struct tm& timeInfo) {
  time_t rawTime;
  time(&rawTime);
  localtime_r(&rawTime, &timeInfo);
}

void User::hexStr(const unsigned char* data, int len, char* buffer) {
  for (int i = 0; i < len; ++i) {
    buffer[2 * i] = hexmap[(data[i] & 0xF0) >> 4];
    buffer[2 * i + 1] = hexmap[data[i] & 0x0F];
  }

  buffer[len * 2] = 0;
}

int User::computeHmacHash(const char* message, unsigned char* hash) {
  return mbedtls_md_hmac(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), (unsigned char*)HMAC_KEY, strlen(HMAC_KEY), (unsigned char*)message, strlen(message), hash);
}

void User::getPermissionsValidityHash(const char* username, uint32_t permissions, const unsigned char* passwordHash, char* hexHash) {
  char message[USERNAME_BUFFER_SIZE + INT32_CHAR_BUFFER_SIZE + 2 * HASH_BUFFER_SIZE + 1] = { 0 };
  strcpy(message, username);
  strcat(message, COOKIE_DELIMITER);
  utoa(permissions, &message[strlen(message)], 10);
  strcat(message, COOKIE_DELIMITER);
  hexStr(passwordHash, HASH_BUFFER_SIZE, &message[strlen(message)]);

  unsigned char hash[HASH_BUFFER_SIZE];
  computeHmacHash(message, hash);
  hexStr(hash, HASH_BUFFER_SIZE, hexHash);
}

void User::composeCookieBase(const char* username, const char* displayname, uint32_t permissions, char* cookieBase, char* hexHash) {
  struct tm timeInfo;
  actTime(timeInfo);
  composeCookieBase(username, displayname, permissions, timeInfo, cookieBase, hexHash);
}

void User::composeCookieBase(const char* username, const char* displayname, uint32_t permissions, struct tm& timeInfo, char* cookieBase, char* hexHash) {
  strcpy(cookieBase, username);
  strcat(cookieBase, COOKIE_DELIMITER);
  strcat(cookieBase, displayname);
  strcat(cookieBase, COOKIE_DELIMITER);
  utoa(permissions, &cookieBase[strlen(cookieBase)], 10);
  strcat(cookieBase, COOKIE_DELIMITER);
  strftime(&cookieBase[strlen(cookieBase)], UTC_TIME_STRING_BUFFER_SIZE, "%Y-%m-%dT%H:%M:%S", &timeInfo);  //2023-02-11T07:37:40
  unsigned char hash[HASH_BUFFER_SIZE];
  computeHmacHash(cookieBase, hash);
  hexStr(hash, HASH_BUFFER_SIZE, hexHash);
}

bool User::createUser(const char* username, const char* displayname, const char* password, uint32_t permissions) {
  unsigned char hash[HASH_BUFFER_SIZE];
  computeHmacHash(password, hash);
  return displayNamesStorage->putString(username, displayname) > 0 && hashesStorage->putBytes(username, hash, HASH_BUFFER_SIZE) > 0 && permissionsStorage->putUInt(username, permissions) > 0 && billsStorage->putUShort(username, 0) > 0;
}

bool User::delteUser(const char* username) {
  bool res = displayNamesStorage->remove(username);
  res = hashesStorage->remove(username) && res;
  res = permissionsStorage->remove(username) && res;
  res = billsStorage->remove(username);
  return res;
}

bool User::verifyPassword(const char* username, const char* password) {
  unsigned char hash1[HASH_BUFFER_SIZE];
  unsigned char hash2[HASH_BUFFER_SIZE];
  computeHmacHash(password, hash1);
  hashesStorage->getBytes(username, hash2, HASH_BUFFER_SIZE);
  return memcmp(hash1, hash2, HASH_BUFFER_SIZE) == 0;
}

bool User::setPassword(const char* username, const char* password) {
  unsigned char hash[HASH_BUFFER_SIZE];
  computeHmacHash(password, hash);
  return hashesStorage->putBytes(username, hash, HASH_BUFFER_SIZE);
}

bool User::getCookie(const char* username, char* cookie) {
  char displayname[USERNAME_BUFFER_SIZE] = { 0 };
  unsigned char passwordHash[HASH_BUFFER_SIZE];
  char cookieBase[COOKIE_BUFFER_SIZE] = { 0 };
  bool res = true;
  
  res = res && displayNamesStorage->getString(username, displayname, USERNAME_BUFFER_SIZE);
  res = res && hashesStorage->getBytes(username, passwordHash, HASH_BUFFER_SIZE);
  if (!res) {
    return false;
  }

  uint32_t permissions = permissionsStorage->getUInt(username, 0);
  //Hash pro ověření cookie
  composeCookieBase(username, displayname, permissions, cookieBase, cookie);
  strcat(cookie, COOKIE_DELIMITER);
  //Hash pro ověření změny hesla a práv
  getPermissionsValidityHash(username, permissions, passwordHash, &cookie[strlen(cookie)]);
  strcat(cookie, COOKIE_DELIMITER);
  strcat(cookie, cookieBase);

  return true;
}

bool User::verifyCookie(const char* cookie) {
  unsigned char hash[HASH_BUFFER_SIZE];
  //computeHMAChash(password, hash);
  return false;
}
int16_t User::getUserBill(const char* username) {
  return billsStorage->getUShort(username) > 0;
}

bool User::setUserBill(const char* username, uint16_t bill) {
  return billsStorage->putUShort(username, 0) > 0;
}

bool User::addUserBill(const char* username, uint16_t add, uint16_t& res) {
  return billsStorage->addUShort(username, add, 0, res);
}

bool User::checkPermissions(uint32_t permissions, uint32_t permissionMask) {
  return permissions & permissionMask;
}

bool User::isPermited(const char* username, uint32_t permissionMask) {
  return checkPermissions(permissionsStorage->getUInt(username), permissionMask);
}