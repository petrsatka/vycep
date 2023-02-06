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

bool Users::createUser(const char* username, const char* displayname, const unsigned char* passwordHash, uint32_t permissions) {
  return displayNamesStorage->putString(username, displayname) > 0 && hashesStorage->putBytes(username, passwordHash, HASH_LEN) > 0 && permissionsStorage->putUInt(username, permissions) > 0 && billsStorage->putUShort(username, 0) > 0;
}

bool Users::delteUser(const char* username) {
  bool res = displayNamesStorage->remove(username);
  res = hashesStorage->remove(username) && res;
  res = permissionsStorage->remove(username) && res;
  res = billsStorage->remove(username);
  return res;
}

bool Users::verifyPasswordHash(const char* username, const unsigned char* passwordHash) {
  return false;
}

bool Users::setPasswordHash(const char* username, const unsigned char* passwordHash) {
  return hashesStorage->putBytes(username, passwordHash, HASH_LEN);
}

bool Users::getUserCookie(const char* username, char* cookie, size_t maxLen) {
  return false;
  //return displayNames->getString(username, cookie, USERNAME_MAX_LEN);
}

bool Users::verifyUserCookie(const char* cookie) {
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

bool Users::isActive(uint32_t permissions) {
  return false;
}

bool Users::isAdmin(uint32_t permissions) {
  return false;
}

bool Users::canPay(uint32_t permissions) {
  return false;
}