#ifndef Utils_h
#define Utils_h
#include <Arduino.h>
#include "mbedtls/md.h"

class Utils {
private:
  static constexpr const char* HMAC_KEY = "iR8sPr11YLDR1Ij7bw2ec70YvpJrt3gK";
  static constexpr const char* HEXMAP = "0123456789abcdef";

public:
  static void actTime(struct tm& timeInfo);
  static void hexStr(const unsigned char* data, int len, char* buffer);
  static int computeHmacHash(const char* message, unsigned char* hash);
};
#endif