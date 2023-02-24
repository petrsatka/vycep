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

int Utils::computeHmacHash(const char* message, unsigned char* hash) {
  return mbedtls_md_hmac(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), (unsigned char*)HMAC_KEY, strlen(HMAC_KEY), (unsigned char*)message, strlen(message), hash);
}

bool Utils::extractCookie(AsyncWebServerRequest *request, char *cookie) {
  if (request->hasHeader("cookie")) {
    char *cookiePos = strstr(request->getHeader("cookie")->value().c_str(), COOKIE_NAME);
    if (cookiePos != NULL && cookiePos[strlen(COOKIE_NAME)] != 0) {
      cookiePos += strlen(COOKIE_NAME);
      strcpy(cookie, cookiePos);
      return true;
    }
  }

  return false;
}

bool Utils::isAuthorized(AsyncWebServerRequest *request, uint32_t permissionMask) {
  char cookie[User::COOKIE_BUFFER_SIZE] = { 0 };
  return extractCookie(request, cookie) && User::isAuthorized(cookie, permissionMask);
}

void Utils::serveStaticAuth(AsyncWebServerRequest *request, const char *path, uint32_t permissionMask) {
  if (isAuthorized(request, permissionMask)) {
    request->send(LittleFS, path);
  } else {
    request->redirect("/login.html");
  }
}

void Utils::onNotFound(AsyncWebServerRequest *request) {
  request->send(404);
}