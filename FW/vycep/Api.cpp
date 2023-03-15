#include "Api.h"

Api::Api(User& user)
  : user(user) {
}

Api::~Api() {
}

constexpr const char Api::credentialsVerificationResultNames[CRED_VERIF_ERR_COUNT][CRED_VERIF_ERR_BUFFER_SIZE];

const char* Api::getCredentialsVerificationResultName(User::CredentialsVerificationResult res) {
  if (static_cast<int>(res) < 0 || static_cast<int>(res) >= CRED_VERIF_ERR_COUNT) {
    return Api::credentialsVerificationResultNames[static_cast<int>(User::CredentialsVerificationResult::UNKNOWN_ERROR)];
  }

  return Api::credentialsVerificationResultNames[static_cast<int>(res)];
}

void Api::serveAuth(AsyncWebServerRequest* request, uint32_t permissionMask, ResponseGetterFunction responseGetter, ErrorResponseGetterFunction noPermissionsresponseGetter, ErrorResponseGetterFunction errorResponseGetter) {
  sprintln("!serveAuth");
  AsyncWebServerResponse* response = NULL;
  char cookie[User::COOKIE_BUFFER_SIZE] = { 0 };
  if (extractCookie(request, AHUTH_COOKIE_NAME, cookie)) {
    char username[User::USERNAME_BUFFER_SIZE] = { 0 };
    uint32_t permissions = 0;
    char newCookie[User::COOKIE_BUFFER_SIZE] = { 0 };

    User::CookieVerificationResult res = this->user.getCookieInfo(cookie, username, &permissions, newCookie);
    if (res == User::CookieVerificationResult::OK || res == User::CookieVerificationResult::OUT_OF_DATE_REVALIDATED) {
      bool forceSetCookie = false;
      //Cookie prošlo, nebo bylo obnoveno
      if (User::checkPermissions(permissions, permissionMask)) {
        //Má oprávnění
        if (responseGetter != NULL) {
          response = responseGetter(username, cookie, newCookie, forceSetCookie);
        }
      } else {
        if (noPermissionsresponseGetter != NULL) {
          //Nemá oprávnění, ale je pro tento případ nastaven fallback
          response = noPermissionsresponseGetter();
        }
      }

      if (response != NULL) {
        if (res == User::CookieVerificationResult::OUT_OF_DATE_REVALIDATED || forceSetCookie) {
          if (!setCookies(response, username, newCookie)) {
            delete (response);
            response = NULL;
          }
        }
      }
    }
  }

  if (response == NULL && errorResponseGetter != NULL) {
    response = errorResponseGetter();
  }

  if (response != NULL) {
    request->send(response);
  } else {
    request->send(401);
  }
}

void Api::serveStaticAuth(AsyncWebServerRequest* request, const char* path, uint32_t permissionMask) {
  dprintln("serveStaticAuth");
  serveAuth(
    request, permissionMask, [request, path](const char* username, const char* cookie, char* newCookie, bool& setCookie) {
      return request->beginResponse(LittleFS, path);
    },
    NULL, [request]() {
      AsyncWebServerResponse* response = request->beginResponse(302);
      response->addHeader("Location", "/login.html");
      return response;
    });
}

void Api::serveDynamicAuth(AsyncWebServerRequest* request, ResponseGetterFunction responseGetter, uint32_t permissionMask) {
  sprintln("!serveDynamicAuth");
  serveAuth(
    request, permissionMask, responseGetter, [request]() {
      //Forbidden
      return request->beginResponse(403);
    },
    NULL);
}

bool Api::extractCookie(AsyncWebServerRequest* request, const char* cookieName, char* cookie) {
  dprintln("extractCookie");
  if (request->hasHeader("cookie")) {
    char* cookiePos = strstr(request->getHeader("cookie")->value().c_str(), cookieName);
    if (cookiePos != NULL && cookiePos[strlen(cookieName)] != 0) {
      cookiePos += strlen(cookieName);
      if (strlen(cookiePos) < User::COOKIE_BUFFER_SIZE) {
        strcpy(cookie, cookiePos);
        return true;
      } else {
        return false;
      }
    }
  }

  return false;
}

bool Api::setCookie(AsyncWebServerResponse* response, const char* name, const char* content, bool httpOnly) {
  dprintln("setCookie");
  if (name == NULL || name[0] == 0 || content == NULL) {
    return false;
  }

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

bool Api::unsetCookies(AsyncWebServerResponse* response) {
  sprintln("!unsetCookies");
  bool res = unsetCookie(response, AHUTH_COOKIE_NAME);
  return unsetCookie(response, USERNAME_COOKIE_NAME) && res;
}

bool Api::unsetCookie(AsyncWebServerResponse* response, const char* name) {
  sprintln("!unsetCookie");
  String cookieStr = String(name);
  bool res = cookieStr.concat(UNSET_COOKIE_ATTRIBUTES);

  if (res) {
    response->addHeader("Set-Cookie", cookieStr);
    return true;
  }

  return false;
}

bool Api::setCookies(AsyncWebServerResponse* response, const char* lCaseUsername, const char* authCookieContent) {
  dprintln("setCookies");
  return setCookie(response, USERNAME_COOKIE_NAME, lCaseUsername, false) && setCookie(response, AHUTH_COOKIE_NAME, authCookieContent, true);
}

void Api::onNotFound(AsyncWebServerRequest* request) {
  sprintln("!onNotFound");
  request->send(404);
}

bool Api::createFirstAdmin(AsyncWebServerRequest* request) {
  dprintln("createFirstAdmin");
  if (request->hasParam("username", true)) {
    AsyncWebParameter* pUname = request->getParam("username", true);
    if (request->hasParam("password", true)) {
      AsyncWebParameter* pPassword = request->getParam("password", true);
      char lCaseUsername[User::USERNAME_BUFFER_SIZE] = { 0 };
      User::CredentialsVerificationResult res = user.registerFirstAdmin(pUname->value().c_str(), pPassword->value().c_str(), lCaseUsername);
      if (res == User::CredentialsVerificationResult::OK) {
        AsyncWebServerResponse* response = request->beginResponse(200, "text/plain", Api::getCredentialsVerificationResultName(res));
        char authCookieContent[User::COOKIE_BUFFER_SIZE] = { 0 };
        user.getNewCookie(lCaseUsername, authCookieContent);
        setCookies(response, lCaseUsername, authCookieContent);
        request->send(response);
        return true;
      } else {
        request->send(200, "text/plain", Api::getCredentialsVerificationResultName(res));
        return false;
      }
    }
  }

  request->send(400);
  return false;
}

bool Api::createUser(AsyncWebServerRequest* request) {
  dprintln("createUser");
  if (request->hasParam("username", true)) {
    AsyncWebParameter* pUname = request->getParam("username", true);
    if (request->hasParam("password", true)) {
      AsyncWebParameter* pPassword = request->getParam("password", true);
      char lCaseUsername[User::USERNAME_BUFFER_SIZE] = { 0 };
      User::CredentialsVerificationResult res = user.registerUser(pUname->value().c_str(), pPassword->value().c_str(), lCaseUsername);
      if (res == User::CredentialsVerificationResult::OK) {
        AsyncWebServerResponse* response = request->beginResponse(200, "text/plain", Api::getCredentialsVerificationResultName(res));
        char authCookieContent[User::COOKIE_BUFFER_SIZE] = { 0 };
        user.getNewCookie(lCaseUsername, authCookieContent);
        setCookies(response, lCaseUsername, authCookieContent);
        request->send(response);
        return true;
      } else {
        request->send(200, "text/plain", Api::getCredentialsVerificationResultName(res));
        return false;
      }
    }
  }

  request->send(400);
  return false;
}

void Api::changePassword(AsyncWebServerRequest* request) {
  sprintln("!changePassword");
  serveDynamicAuth(
    request, [&](const char* lCaseUsername, const char* cookie, char* newCookie, bool& setCookie) {
      if (request->hasParam("oldpassword", true)) {
        AsyncWebParameter* pOldPassword = request->getParam("oldpassword", true);
        if (request->hasParam("newpassword", true)) {
          AsyncWebParameter* pNewPassword = request->getParam("newpassword", true);
          AsyncWebServerResponse* response = request->beginResponse(
            200,
            "text/plain",
            Api::getCredentialsVerificationResultName(user.changePassword(lCaseUsername, pOldPassword->value().c_str(), pNewPassword->value().c_str())));
          setCookie = user.getNewCookie(lCaseUsername, newCookie);
          return response;
        }
      }

      return request->beginResponse(400);
    },
    User::PERMISSIONS_ACTIVE);
}

void Api::getCurrentUserBillCount(AsyncWebServerRequest* request) {
  sprintln("!getCurrentUserBillCount");
  serveDynamicAuth(
    request, [&](const char* lCaseUsername, const char* cookie, char* newCookie, bool& setCookie) {
      AsyncWebServerResponse* response = request->beginResponse(
        200,
        "text/plain",
        String(GENERAL_SUCCESS_RESULT_CODE) + "&" + String(user.getUserBill(lCaseUsername)));
      return response;
    },
    User::PERMISSIONS_ANY_PERMISSIONS);
}

bool Api::login(AsyncWebServerRequest* request) {
  sprintln("!login");
  //AsyncWebServerResponse* response = request->beginResponse(200, "text/plain", GENERAL_SUCCESS_RESULT_CODE);
  //unsetCookies(response);
  //request->send(response);
  if (request->hasParam("username", true)) {
    AsyncWebParameter* pUname = request->getParam("username", true);
    if (request->hasParam("password", true)) {
      AsyncWebParameter* pPassword = request->getParam("password", true);
      char lCaseUsername[User::USERNAME_BUFFER_SIZE] = { 0 };
      if (user.verifyPassword(pUname->value().c_str(), pPassword->value().c_str(), lCaseUsername) == User::CredentialsVerificationResult::OK) {
        AsyncWebServerResponse* response = request->beginResponse(200, "text/plain", GENERAL_SUCCESS_RESULT_CODE);
        char authCookieContent[User::COOKIE_BUFFER_SIZE] = { 0 };
        user.getNewCookie(lCaseUsername, authCookieContent);
        setCookies(response, lCaseUsername, authCookieContent);
        request->send(response);
        return true;
      } else {
        request->send(200, "text/plain", INVALID_USERNAME_OR_PASSWORD_RESULT_CODE);
        return false;
      }
    }
  }

  request->send(400);
  return false;
}

void Api::getQueueCount(AsyncWebServerRequest* request) {
  sprintln("!getQueueCount");
}

void Api::getUserBillCount(AsyncWebServerRequest* request) {
  sprintln("!getUserBillCount");
  //Ověřit práva admina
}

void Api::makeOrder(AsyncWebServerRequest* request) {
  sprintln("!makeOrder");
  //Username získat z cookies
  //Ověřit, zda je uživatel schválen
}

void Api::pay(AsyncWebServerRequest* request) {
  sprintln("!pay");
  //Username získat z cookies
  //Ověřit, zda je uživatel schválen a má práva na placení
}

void Api::payForUser(AsyncWebServerRequest* request) {
  sprintln("!payForUser");
  //username
  //Ověřit, zda má práva admina
}

void Api::loadUsers(AsyncWebServerRequest* request) {
  sprintln("!loadUsers");
  //Ověřit práva admina
}

void Api::logout(AsyncWebServerRequest* request) {
  dprintln("logout");
  AsyncWebServerResponse* response = request->beginResponse(200, "text/plain", GENERAL_SUCCESS_RESULT_CODE);
  unsetCookies(response);
  request->send(response);
}