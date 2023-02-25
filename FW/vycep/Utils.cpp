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

size_t Utils::appendChar(char* str, char a)
{
    size_t len = strlen(str);   
    str[len] = a;
    str[++len] = 0;

    return len;
}

int Utils::computeHmacHash(const char* message, unsigned char* hash) {
  return mbedtls_md_hmac(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), (unsigned char*)HMAC_KEY, strlen(HMAC_KEY), (unsigned char*)message, strlen(message), hash);
}

bool Utils::extractCookie(AsyncWebServerRequest* request, const char* cookieName, char* cookie) {
  if (request->hasHeader("cookie")) {
    char* cookiePos = strstr(request->getHeader("cookie")->value().c_str(), cookieName);
    if (cookiePos != NULL && cookiePos[strlen(cookieName)] != 0) {
      cookiePos += strlen(cookieName);
      strcpy(cookie, cookiePos);
      return true;
    }
  }

  return false;
}

bool Utils::setCookie(AsyncWebServerResponse* response, const char* name, const char* content, bool httpOnly) {
  String cookieStr = String(name);
  bool res = cookieStr.concat(content) && cookieStr.concat(COMMON_COOKIE_ATTRIBUTES);
  if (httpOnly) {
    res = res && cookieStr.concat(HTTPONLY_COOKIE_ATTRIBUTE);
  }

  if (res) {
    response->addHeader("Set-Cookie", cookieStr);
    return true;
  }

  return false;
}

bool Utils::setCookies(AsyncWebServerResponse* response, const char* displayname, const char* authCookieContent) {
  return setCookie(response, DISPLAYNAME_COOKIE_NAME, displayname, false) && setCookie(response, AHUTH_COOKIE_NAME, authCookieContent, true);
}

bool Utils::isAuthorized(AsyncWebServerRequest* request, uint32_t permissionMask) {
  char cookie[User::COOKIE_BUFFER_SIZE] = { 0 };
  return extractCookie(request, AHUTH_COOKIE_NAME, cookie) && User::isAuthorized(cookie, permissionMask);
}

void Utils::serveStaticAuth(AsyncWebServerRequest* request, const char* path, uint32_t permissionMask) {
  if (isAuthorized(request, permissionMask)) {
    request->send(LittleFS, path);
  } else {
    request->redirect("/login.html");
  }
}

void Utils::onNotFound(AsyncWebServerRequest* request) {
  request->send(404);
}