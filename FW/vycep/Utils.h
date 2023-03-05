#ifndef Utils_h
#define Utils_h
#include <Arduino.h>
#include <LittleFS.h>
#include <ESPAsyncWebSrv.h>
#include "mbedtls/md.h"
//#include "User.h"
#include "Debug.h"

class Utils {
public:
  static constexpr size_t HASH_BUFFER_SIZE = 32;
  static constexpr size_t HASH_HEXSTRING_BUFFER_SIZE = 2 * HASH_BUFFER_SIZE + 1;

  static void actTime(struct tm& timeInfo);
  static void hexStr(const unsigned char* data, int len, char* buffer);
  static size_t appendChar(char* str, char a);
  static int computeHmacHash(const char* message, unsigned char* hash);
  static bool isAlphaNumericStr(const char* str);
  static bool toLowerStr(const char* inStr, char* outStr, size_t bufferSize);
  static int strLenUTF8(const char* str);

  static void test();

private:
  static constexpr const char* HMAC_KEY = "iR8sPr11YLDR1Ij7bw2ec70YvpJrt3gK";
  static constexpr const char* HEXMAP = "0123456789abcdef";
};
#endif