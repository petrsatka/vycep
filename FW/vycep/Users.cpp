#include "Users.h"

Users::Users() {
  xSemaphore = xSemaphoreCreateMutex();
  displayNamesStorage = new TSafePreferences(this->xSemaphore, NAMESPACE_DISPLAY_NAMES, NVS_PARTTION);
  hashesStorage = new TSafePreferences(this->xSemaphore, NAMESPACE_HASHES, NVS_PARTTION);
  permissionsStorage = new TSafePreferences(this->xSemaphore, NAMESPACE_PERMISSIONS, NVS_PARTTION);
  billsStorage = new TSafePreferences(this->xSemaphore, NAMESPACE_BILLS, NVS_PARTTION);
 }

Users::~Users() {
  delete (displayNamesStorage);
  delete (hashesStorage);
  delete (permissionsStorage);
  delete (billsStorage);
  vSemaphoreDelete(xSemaphore);
}

void Users::hexStr(unsigned char *data, int len, char* buffer)
{
  for (int i = 0; i < len; ++i) {
    buffer[2 * i]     = hexmap[(data[i] & 0xF0) >> 4];
    buffer[2 * i + 1] = hexmap[data[i] & 0x0F];
  }

  buffer[len * 2] = 0;
}

int Users::computeHMAChash(const char* message, unsigned char* hash) {
  return mbedtls_md_hmac(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), (unsigned char*)HMAC_KEY, strlen(HMAC_KEY), (unsigned char*)message, strlen(message), hash);
}

void Users::composeCookieBase(const char* username, const char* displayname, uint32_t permissions, struct tm* time, char* cookieBase) {
  const short number32bitBufferSize = 11;
  const short utcTimeStrinBufferSize = 26;

  char permissionsChars[number32bitBufferSize] = { 0 };
  utoa(permissions, permissionsChars, 10);

  char utcTimeString[utcTimeStrinBufferSize] = { 0 };
  //2023-02-11T07:37:40+00:00
  strftime(utcTimeString, utcTimeStrinBufferSize, "%Y-%m-%dT%H:%M:%S%z", time);

  strcpy(cookieBase, username);
  strcat(cookieBase, COOKIE_DELIMITER);
  strcat(cookieBase, displayname);
  strcat(cookieBase, COOKIE_DELIMITER);
  strcat(cookieBase, permissionsChars);
  strcat(cookieBase, COOKIE_DELIMITER);
  strcat(cookieBase, utcTimeString);
}

bool Users::createUser(const char* username, const char* displayname, const char* password, uint32_t permissions) {
  unsigned char hash[HASH_BUFFER_SIZE];
  computeHMAChash(password, hash);
  return displayNamesStorage->putString(username, displayname) > 0 && hashesStorage->putBytes(username, hash, HASH_BUFFER_SIZE) > 0 && permissionsStorage->putUInt(username, permissions) > 0 && billsStorage->putUShort(username, 0) > 0;
}

bool Users::delteUser(const char* username) {
  bool res = displayNamesStorage->remove(username);
  res = hashesStorage->remove(username) && res;
  res = permissionsStorage->remove(username) && res;
  res = billsStorage->remove(username);
  return res;
}

bool Users::verifyPassword(const char* username, const char* password) {
  unsigned char hash1[HASH_BUFFER_SIZE];
  unsigned char hash2[HASH_BUFFER_SIZE];
  computeHMAChash(password, hash1);
  hashesStorage->getBytes(username, hash2, HASH_BUFFER_SIZE);
  return memcmp(hash1, hash2, HASH_BUFFER_SIZE) == 0;
}

bool Users::setPassword(const char* username, const char* password) {
  unsigned char hash[HASH_BUFFER_SIZE];
  computeHMAChash(password, hash);
  return hashesStorage->putBytes(username, hash, HASH_BUFFER_SIZE);
}

bool Users::getUserCookie(const char* username, char* cookie) {
  char displayname[USERNAME_BUFFER_SIZE] = { 0 };
  unsigned char passwordHash[HASH_BUFFER_SIZE];
  char cookieBase[COOKIE_BUFFER_SIZE] = { 0 };
  /*
  username
displayname
permissions
creationDate
permchecksum sha256(username + passwordhash + permissions + secKey)
validitycheckusm sha256(username + permissions + creationDate + secKey)
  cookie = username + " " + displayname + " " + permissoins + " " + creationDate + " " + checksum(cookie) + validityCheckusum(username + passwordhash + permissions + secKey)
  16 + 16 + 11 + 26 + 65 + 64 +1
  */
  //computeHMAChash(password, hash);
  bool res = true;
  res = res && displayNamesStorage->getString(username, displayname, USERNAME_BUFFER_SIZE);
  res = res && hashesStorage->getBytes(username, passwordHash, HASH_BUFFER_SIZE);
  if (!res) {
    return false;
  }

  uint32_t permissions = permissionsStorage->getUInt(username, 0);

  time_t rawtime;
  struct tm* ptm;
  time(&rawtime);
  ptm = gmtime(&rawtime);

  composeCookieBase(username, displayname, permissions, ptm, cookieBase);
  
  unsigned char hash[HASH_BUFFER_SIZE];
  computeHMAChash(cookieBase, hash);

  char hexHash[2 * HASH_BUFFER_SIZE + 1];
  hexStr(hash, HASH_BUFFER_SIZE, hexHash);
  strcpy(cookie, hexHash);

  return false;
  //return displayNames->getString(username, cookie, USERNAME_MAX_LEN);
}

bool Users::verifyUserCookie(const char* cookie) {
  unsigned char hash[HASH_BUFFER_SIZE];
  //computeHMAChash(password, hash);
  return false;
}
int16_t Users::getUserBill(const char* username) {
  return billsStorage->getUShort(username) > 0;
}

bool Users::setUserBill(const char* username, uint16_t bill) {
  return billsStorage->putUShort(username, 0) > 0;
}

bool Users::addUserBill(const char* username, uint16_t add, uint16_t& res) {
  return billsStorage->addUShort(username, add, 0, res);
}

bool Users::checkPermissions(uint32_t permissions, uint32_t permissionMask) {
  return permissions & permissionMask;
}

bool Users::isPermited(const char* username, uint32_t permissionMask) {
  return checkPermissions(permissionsStorage->getUInt(username), permissionMask);
}