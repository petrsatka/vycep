#ifndef Utils_h
#define Utils_h
#include <Arduino.h>
#include <LittleFS.h>
#include <ESPAsyncWebSrv.h>
#include "mbedtls/md.h"
#include "User.h"

class Utils {
public:
  static constexpr const char* AHUTH_COOKIE_NAME = "ESPAUTH=";
  static constexpr const char* DISPLAYNAME_COOKIE_NAME = "ESPDISPNAME=";

  static void actTime(struct tm& timeInfo);
  static void hexStr(const unsigned char* data, int len, char* buffer);
  static size_t appendChar(char* str, char a);
  static int computeHmacHash(const char* message, unsigned char* hash);
  static bool isAuthorized(AsyncWebServerRequest* request, uint32_t permissionMask);
  static void serveStaticAuth(AsyncWebServerRequest* request, const char* path, uint32_t permissionMask);
  static void onNotFound(AsyncWebServerRequest* request);
  static bool extractCookie(AsyncWebServerRequest* request, const char* cookieName, char* cookie);
  static bool setCookies(AsyncWebServerResponse* response, const char* displayname, const char* authCookieContent);

private:
  static constexpr const char* HMAC_KEY = "iR8sPr11YLDR1Ij7bw2ec70YvpJrt3gK";
  static constexpr const char* HEXMAP = "0123456789abcdef";
  static constexpr const char* HTTPONLY_COOKIE_ATTRIBUTE = "; HttpOnly";
  static constexpr const char* COMMON_COOKIE_ATTRIBUTES = "; Max-Age=1707109200; Path=/";

  static bool setCookie(AsyncWebServerResponse* response, const char* name, const char* content, bool httpOnly);
};
#endif