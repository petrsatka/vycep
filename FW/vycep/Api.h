#ifndef Api_h
#define Api_h
#include <Arduino.h>
#include <ESPAsyncWebSrv.h>
#include "User.h"
#include "Utils.h"
#include "Debug.h"

typedef std::function<AsyncWebServerResponse*(const char* lCaseUsername, const char* cookie, char* newCookie, bool &setCookie)> ResponseGetterFunction;
typedef std::function<AsyncWebServerResponse*()> ErrorResponseGetterFunction;

class Api {
public:
  Api(User& user);
  ~Api();
  static constexpr const char* AHUTH_COOKIE_NAME = "ESPAUTH=";
  static constexpr const char* USERNAME_COOKIE_NAME = "ESPUNAME=";
  static constexpr const char* GENERAL_SUCCESS_RESULT_CODE = "OK";
  static constexpr const char* INVALID_USERNAME_OR_PASSWORD_RESULT_CODE = "INVALID_USERNAME_OR_PASSWORD";

  void serveStaticAuth(AsyncWebServerRequest* request, const char* path, uint32_t permissionMask);
  void serveDynamicAuth(AsyncWebServerRequest* request, ResponseGetterFunction responseGetter, uint32_t permissionMask);
  void serveAuth(AsyncWebServerRequest* request, uint32_t permissionMask, ResponseGetterFunction responseGetter, ErrorResponseGetterFunction noPermissionsresponseGetter, ErrorResponseGetterFunction errorResponseGetter);
  bool createFirstAdmin(AsyncWebServerRequest* request);
  bool createUser(AsyncWebServerRequest* request);
  void changePassword(AsyncWebServerRequest* request);
  bool login(AsyncWebServerRequest* request);
  void getQueueCount(AsyncWebServerRequest* request);
  void getCurrentUserBillCount(AsyncWebServerRequest* request);
  void getUserBillCount(AsyncWebServerRequest* request);
  void makeOrder(AsyncWebServerRequest* request);
  void pay(AsyncWebServerRequest* request);
  void payForUser(AsyncWebServerRequest* request);
  void loadUsers(AsyncWebServerRequest* request);
  void logout(AsyncWebServerRequest* request);

  static void onNotFound(AsyncWebServerRequest* request);

private:
  User& user;

  static constexpr int CRED_VERIF_ERR_COUNT = 13;
  static constexpr int CRED_VERIF_ERR_BUFFER_SIZE = 32;
  static constexpr const char* HTTPONLY_COOKIE_ATTRIBUTE = "; HttpOnly";
  static constexpr const char* COMMON_COOKIE_ATTRIBUTES = "; Max-Age=1707109200; Path=/";
  static constexpr const char* UNSET_COOKIE_ATTRIBUTES = "; Max-Age=-1; Path=/";

  static constexpr const char credentialsVerificationResultNames[CRED_VERIF_ERR_COUNT][CRED_VERIF_ERR_BUFFER_SIZE] = {
    "OK",
    "USERNAME_SHORT",
    "USERNAME_LONG",
    "USERNAME_INVALID_CHARACTERS",
    "USERNAME_EMPTY",
    "PASSWORD_SHORT",
    "PASSWORD_LONG",
    "PASSWORD_EMPTY",
    "USERNAME_EXISTS",
    "UNKNOWN_ERROR",
    "ANY_USER_EXISTS",
    "USERNAME_NOT_EXISTS",
    "INVALID_PASSWORD"
  };

  static const char* getCredentialsVerificationResultName(User::CredentialsVerificationResult res);
  static bool extractCookie(AsyncWebServerRequest* request, const char* cookieName, char* cookie);
  static bool setCookies(AsyncWebServerResponse* response, const char* lCaseUsername, const char* authCookieContent);
  static bool unsetCookies(AsyncWebServerResponse* response);
  static bool setCookie(AsyncWebServerResponse* response, const char* name, const char* content, bool httpOnly);
  static bool unsetCookie(AsyncWebServerResponse* response, const char* name);
};
#endif