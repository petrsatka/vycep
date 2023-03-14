#ifndef Api_h
#define Api_h
#include <Arduino.h>
#include <ESPAsyncWebSrv.h>
#include "User.h"
#include "Utils.h"
#include "Debug.h"

typedef std::function<AsyncWebServerResponse*()> ResponseGetterFunction;
typedef std::function<void()> ErrorResponseFunction;

class Api {
public:
  Api(User& user);
  ~Api();
  static constexpr const char* AHUTH_COOKIE_NAME = "ESPAUTH=";
  static constexpr const char* USERNAME_COOKIE_NAME = "ESPUNAME=";

  void serveStaticAuth(AsyncWebServerRequest* request, const char* path, uint32_t permissionMask);
  void serveDynamicAuth(AsyncWebServerRequest* request, ResponseGetterFunction responseGetter, uint32_t permissionMask);
  void serveAuth(AsyncWebServerRequest* request, uint32_t permissionMask, ResponseGetterFunction responseGetter, ResponseGetterFunction noPermissionsresponseGetter, ErrorResponseFunction);
  bool createFirstAdmin(AsyncWebServerRequest* request);
  bool createUser(AsyncWebServerRequest* request);
  void changePassword(AsyncWebServerRequest* request);
  void login(AsyncWebServerRequest* request);
  void getQueueCount(AsyncWebServerRequest* request);
  void getBillCount(AsyncWebServerRequest* request);
  void getUserBillCount(AsyncWebServerRequest* request);
  void makeOrder(AsyncWebServerRequest* request);
  void pay(AsyncWebServerRequest* request);
  void payForUser(AsyncWebServerRequest* request);
  void loadUsers(AsyncWebServerRequest* request);
  void logout(AsyncWebServerRequest* request);

  static void onNotFound(AsyncWebServerRequest* request);

private:
  User& user;

  static constexpr const char* HTTPONLY_COOKIE_ATTRIBUTE = "; HttpOnly";
  static constexpr const char* COMMON_COOKIE_ATTRIBUTES = "; Max-Age=1707109200; Path=/";
  static constexpr const char* UNSET_COOKIE_ATTRIBUTES = "; Max-Age=-1; Path=/";

  static bool extractCookie(AsyncWebServerRequest* request, const char* cookieName, char* cookie);
  static bool setCookies(AsyncWebServerResponse* response, const char* lCaseUsername, const char* authCookieContent);
  static bool unsetCookies(AsyncWebServerResponse* response);
  static bool setCookie(AsyncWebServerResponse* response, const char* name, const char* content, bool httpOnly);
  static bool unsetCookie(AsyncWebServerResponse* response, const char* name);
};
#endif