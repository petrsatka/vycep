#include "Utils.h"
void Utils::test() {
  unsigned char tArr[256];
  char refStr[] = "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9fa0a1a2a3a4a5a6a7a8a9aaabacadaeafb0b1b2b3b4b5b6b7b8b9babbbcbdbebfc0c1c2c3c4c5c6c7c8c9cacbcccdcecfd0d1d2d3d4d5d6d7d8d9dadbdcdddedfe0e1e2e3e4e5e6e7e8e9eaebecedeeeff0f1f2f3f4f5f6f7f8f9fafbfcfdfeff";
  for (int i = 0; i < 256; i++) {
    tArr[i] = i;
  }

  char buff[256 * 2 + 1];
  Utils::hexStr(tArr, sizeof(tArr), buff);
  if (strcmp(buff, refStr) == 0) {
    sprintln("HexStr test 1 OK");
  } else {
    sprintln("HexStr test 1 FAILED");
  }

  char buff2[1000];
  Utils::hexStr(tArr, sizeof(tArr), buff2);
  if (strcmp(buff2, refStr) == 0) {
    sprintln("HexStr test 2 OK");
  } else {
    sprintln("HexStr test 2 FAILED");
  }

  unsigned char hash[Utils::HASH_BUFFER_SIZE];
  Utils::computeHmacHash("abcdefghijklop", hash);
  char hexhash[Utils::HASH_HEXSTRING_BUFFER_SIZE];
  Utils::hexStr(hash, sizeof(hash), hexhash);
  if (strcmp(hexhash, "97ee2acc08dd69ac9879e5b22b71544b79ea3997bc28dfd878948a125f857928") == 0) {
    sprintln("computeHmacHash test OK");
  } else {
    sprintln("computeHmacHash test FAILED");
  }
}

void Utils::actTime(struct tm& timeInfo) {
  sprintln("!actTime");
  time_t rawTime;
  time(&rawTime);
  localtime_r(&rawTime, &timeInfo);
}

void Utils::hexStr(const unsigned char* data, int len, char* buffer) {
  dprintln("hexStr");
  for (int i = 0; i < len; ++i) {
    buffer[2 * i] = HEXMAP[(data[i] & 0xF0) >> 4];
    buffer[2 * i + 1] = HEXMAP[data[i] & 0x0F];
  }

  buffer[len * 2] = 0;
}

size_t Utils::appendChar(char* str, char a) {
  sprintln("!appendChar");
  size_t len = strlen(str);
  str[len] = a;
  str[++len] = 0;

  return len;
}

int Utils::computeHmacHash(const char* message, unsigned char* hash) {
  dprintln("computeHmacHash");
  return mbedtls_md_hmac(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), (unsigned char*)HMAC_KEY, strlen(HMAC_KEY), (unsigned char*)message, strlen(message), hash);
}

bool Utils::isAlphaNumericStr(const char* str) {
  sprintln("!isAlphaNumericStr");
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
  sprintln("!toLowerStr");
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
  sprintln("!strLenUTF8");
  int len = 0;
  if (str != NULL) {
    while (*str) {
      //Spočítá první bajty
      len += (*str++ & 0xc0) != 0x80;
    }
  }

  return len;
}