/*
By Petr Satka
Licnese CC-BY
*/

#ifndef Utils_h
#define Utils_h
#include <Arduino.h>
#include <LittleFS.h>
#include <ESPAsyncWebSrv.h>
#include "mbedtls/md.h"
#include "Debug.h"

class Utils {
public:
  static constexpr size_t HASH_BUFFER_SIZE = 32;
  static constexpr size_t HASH_HEXSTRING_BUFFER_SIZE = 2 * HASH_BUFFER_SIZE + 1;
  static constexpr size_t SSID_BUFFER_SIZE = 33;          //32 Znaků
  static constexpr size_t SECURITY_KEY_BUFFER_SIZE = 64;  //63 Znaků

  static void actTime(struct tm& timeInfo);
  static void hexStr(const unsigned char* data, int len, char* buffer);
  static size_t appendChar(char* str, char a);
  static int computeHmacHash(const char* message, unsigned char* hash);
  static bool isAlphaNumericStr(const char* str);
  static bool toLowerStr(const char* inStr, char* outStr, size_t bufferSize);
  static int strLenUTF8(const char* str);
  static short getCookiePropsPosition(const char* cookie);
  static const char* bToStr(bool val);
  static bool StrTob(const char* val);
  static void getRandomString(char *str, unsigned int bufferSize);
  //static void trimRight(char* str);

  static void test();
  static constexpr const char* TRUE_STR = "true";
  static constexpr const char* FALSE_STR = "false";
  static constexpr const char *DEFAULT_HOST_NAME = "vycep";

private:
  static constexpr const char* HMAC_KEY = "iR8sPr11YLDR1Ij7bw2ec70YvpJrt3gK";
  static constexpr const char* HEXMAP = "0123456789abcdef";
};
#endif