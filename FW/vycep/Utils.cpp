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