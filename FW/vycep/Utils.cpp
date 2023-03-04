#include "Utils.h"
void Utils::actTime(struct tm& timeInfo) {
  time_t rawTime;
  time(&rawTime);
  localtime_r(&rawTime, &timeInfo);
}

void Utils::hexStr(const unsigned char* data, int len, char* buffer) {
  for (int i = 0; i < len; ++i) {
    buffer[2 * i] = HEXMAP[(data[i] & 0xF0) >> 4];
    buffer[2 * i + 1] = HEXMAP[data[i] & 0x0F];
  }

  buffer[len * 2] = 0;
}

size_t Utils::appendChar(char* str, char a) {
  size_t len = strlen(str);
  str[len] = a;
  str[++len] = 0;

  return len;
}

int Utils::computeHmacHash(const char* message, unsigned char* hash) {
  return mbedtls_md_hmac(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), (unsigned char*)HMAC_KEY, strlen(HMAC_KEY), (unsigned char*)message, strlen(message), hash);
}

bool Utils::isAlphaNumericStr(const char* str) {
  if (str == NULL) {
    return false;
  }

  bool res = true;
  for (int i = 0; str[i] != 0; i++) {
    res = res && isAlphaNumeric(str[i]);
  }

  return res;
}

bool Utils::toLowerStr(const char* inStr, char* outStr, size_t bufferSize) {
  if (inStr != NULL) {
    int i = 0;
    while (inStr[i] != 0 && i < bufferSize - 1) {
      outStr[i] = tolower(inStr[i]);
      i++;
    }

    outStr[i] = 0;

    return true;
  }

  return false;
}

int Utils::strLenUTF8(const char* str) {
  int len = 0;
  if (str != NULL) {
    while (*str) {
      //Spočítá první bajty
      len += (*str++ & 0xc0) != 0x80;
    }
  }

  return len;
}