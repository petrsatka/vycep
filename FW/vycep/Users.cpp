#include "Users.h"

Users::Users() {
  xSemaphore = xSemaphoreCreateMutex();
  displayNamesStorage = new TSafePreferences(this->xSemaphore, NAMESPACE_DISPLAY_NAMES, NVS_PARTTION);
  hashesStorage = new TSafePreferences(this->xSemaphore, NAMESPACE_HASHES, NVS_PARTTION);
  permissionsStorage = new TSafePreferences(this->xSemaphore, NAMESPACE_PERMISSIONS, NVS_PARTTION);
  billsStorage = new TSafePreferences(this->xSemaphore, NAMESPACE_BILLS, NVS_PARTTION);
  mbedtls_md_init(&hashCtx);
  mbedtls_md_setup(&hashCtx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 1);
  mbedtls_md_hmac_starts(&hashCtx, (unsigned char*)HMAC_KEY, strlen(HMAC_KEY));
}

Users::~Users() {
  delete (displayNamesStorage);
  delete (hashesStorage);
  delete (permissionsStorage);
  delete (billsStorage);
  vSemaphoreDelete(xSemaphore);
  mbedtls_md_free(&hashCtx);
}

int Users::computeHMAChash(const char* message, unsigned char* hash) {
  mbedtls_md_hmac_reset(&hashCtx);
  mbedtls_md_hmac_update(&hashCtx, (unsigned char*)message, strlen(message));
  return mbedtls_md_hmac_finish(&hashCtx, hash);
}

bool Users::createUser(const char* username, const char* displayname, const char* password, uint32_t permissions) {
  unsigned char hash[HASH_LEN];
  computeHMAChash(password, hash);
  return displayNamesStorage->putString(username, displayname) > 0 && hashesStorage->putBytes(username, hash, HASH_LEN) > 0 && permissionsStorage->putUInt(username, permissions) > 0 && billsStorage->putUShort(username, 0) > 0;
}

bool Users::delteUser(const char* username) {
  bool res = displayNamesStorage->remove(username);
  res = hashesStorage->remove(username) && res;
  res = permissionsStorage->remove(username) && res;
  res = billsStorage->remove(username);
  return res;
}

bool Users::verifyPassword(const char* username, const char* password) {
  unsigned char hash[HASH_LEN];
  computeHMAChash(password, hash);
  //for (int i = 0; i < HASH_LEN; i++) Serial.print(hash[i], HEX);
  //Serial.println();
  return false;
}

bool Users::setPassword(const char* username, const char* password) {
  unsigned char hash[HASH_LEN];
  computeHMAChash(password, hash);
  return hashesStorage->putBytes(username, hash, HASH_LEN);
}

bool Users::getUserCookie(const char* username, char* cookie, size_t maxLen) {
  unsigned char hash[HASH_LEN];
  //computeHMAChash(password, hash);
  return false;
  //return displayNames->getString(username, cookie, USERNAME_MAX_LEN);
}

bool Users::verifyUserCookie(const char* cookie) {
  unsigned char hash[HASH_LEN];
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