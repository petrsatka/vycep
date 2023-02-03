#ifndef Users_h
#define Users_h
#include <Arduino.h>
#include "TSafePreferences.h"
class Users {
public:
  Users();
  bool createUser(char* username, char* displayName, unsigned char* sha256byteHash, int32_t permissions);
  bool delteUser(char* username);
  bool verifyPasswordHash(char* username, unsigned char* sha256byteHash);
  bool setPasswordHash(unsigned char* sha256byteHash);
  bool getUserCookie(char* username, char* cookie);
  bool verifyUserCookie(char* cookie);
  int16_t getUserBill(char* username);
  bool setUserBill(char* username, int16_t bill);
  bool addUserBill(char* username, int16_t add, int16_t &res);
  bool isActive(int32_t permissions);
  bool isAdmin(int32_t permissions);
  bool canPay(int32_t permissions);
private:
}
#endif