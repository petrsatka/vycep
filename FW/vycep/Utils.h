#ifndef Utils_h
#define Utils_h
#include <Arduino.h>
#include <LittleFS.h>
#include <ESPAsyncWebSrv.h>
#include "mbedtls/md.h"
#include "User.h"

class Utils {
public:
  static constexpr const char* COOKIE_NAME = "ESPAUTH=";

  static void actTime(struct tm& timeInfo);
  static void hexStr(const unsigned char* data, int len, char* buffer);
  static int computeHmacHash(const char* message, unsigned char* hash);
  static bool isAuthorized(AsyncWebServerRequest* request, uint32_t permissionMask);
  static void serveStaticAuth(AsyncWebServerRequest* request, const char* path, uint32_t permissionMask);
  static void onNotFound(AsyncWebServerRequest* request);
  static bool extractCookie(AsyncWebServerRequest* request, char* cookie);
  //Dodělat nastavení cookie - permanentní, alespoň nějak zabezpečené
private:
  static constexpr const char* HMAC_KEY = "iR8sPr11YLDR1Ij7bw2ec70YvpJrt3gK";
  static constexpr const char* HEXMAP = "0123456789abcdef";
};
#endif