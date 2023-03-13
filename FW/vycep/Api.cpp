#include "Api.h"

Api::Api(User& user)
  : user(user) {
}

Api::~Api() {
}

void Api::serveAuth(AsyncWebServerRequest* request, uint32_t permissionMask, ResponseGetterFunction responseGetter, ResponseGetterFunction noPermissionsresponseGetter, ErrorResponseFunction errorResponse) {
  dprintln("serveAuth");
  char cookie[User::COOKIE_BUFFER_SIZE] = { 0 };
  if (extractCookie(request, AHUTH_COOKIE_NAME, cookie)) {
    char username[User::USERNAME_BUFFER_SIZE] = { 0 };
    uint32_t permissions = 0;
    char newCookie[User::COOKIE_BUFFER_SIZE] = { 0 };

    User::CookieVerificationResult res = this->user.getCookieInfo(cookie, username, &permissions, newCookie);
    if (res == User::CookieVerificationResult::OK || res == User::CookieVerificationResult::OUT_OF_DATE_REVALIDATED) {
      //Cookie prošlo, nebo bylo obnoveno
      AsyncWebServerResponse* response = NULL;
      if (User::checkPermissions(permissions, permissionMask)) {
        //Má oprávnění
        response = responseGetter();
      } else {
        if (noPermissionsresponseGetter != NULL) {
          //Nemá oprávnění, ale je pro tento případ nastaven fallback
          response = noPermissionsresponseGetter();
        }
      }

      if (response != NULL) {
        if (res == User::CookieVerificationResult::OUT_OF_DATE_REVALIDATED) {
          if (setCookies(response, username, newCookie)) {
            request->send(response);
            return;
          } else {
            delete (response);
          }
        } else {
          request->send(response);
          return;
        }
      }
    }
  }

  errorResponse();
}

void Api::serveStaticAuth(AsyncWebServerRequest* request, const char* path, uint32_t permissionMask) {
  dprintln("serveStaticAuth");
  serveAuth(
    request, permissionMask, [request, path]() {
      return request->beginResponse(LittleFS, path);
    },
    NULL, [request]() {
      request->redirect("/login.html");
    });
}

void Api::serveDynamicAuth(AsyncWebServerRequest* request, ResponseGetterFunction responseGetter, uint32_t permissionMask) {
  sprintln("!serveDynamicAuth");
  serveAuth(
    request, permissionMask, responseGetter, [request]() {
      //Forbidden
      request->send(403);
      return nullptr;
    },
    [request]() {
      //Unauthorized
      request->send(401);
    });
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
      switch (res) {
        case User::CredentialsVerificationResult::OK:
          {
            AsyncWebServerResponse* response = request->beginResponse(200, "text/plain", "OK");
            char authCookieContent[User::COOKIE_BUFFER_SIZE] = { 0 };
            user.getNewCookie(lCaseUsername, authCookieContent);
            setCookies(response, lCaseUsername, authCookieContent);
            request->send(response);
            return true;
          }
        case User::CredentialsVerificationResult::USERNAME_SHORT:
          request->send(200, "text/plain", "USERNAME_SHORT");
          break;
        case User::CredentialsVerificationResult::USERNAME_LONG:
          request->send(200, "text/plain", "USERNAME_LONG");
          break;
        case User::CredentialsVerificationResult::USERNAME_INVALID_CHARACTERS:
          request->send(200, "text/plain", "USERNAME_INVALID_CHARACTERS");
          break;
        case User::CredentialsVerificationResult::USERNAME_EMPTY:
          request->send(200, "text/plain", "USERNAME_EMPTY");
          break;
        case User::CredentialsVerificationResult::PASSWORD_SHORT:
          request->send(200, "text/plain", "PASSWORD_SHORT");
          break;
        case User::CredentialsVerificationResult::PASSWORD_LONG:
          request->send(200, "text/plain", "PASSWORD_LONG");
          break;
        case User::CredentialsVerificationResult::PASSWORD_EMPTY:
          request->send(200, "text/plain", "PASSWORD_EMPTY");
          break;
        case User::CredentialsVerificationResult::USERNAME_EXISTS:
          request->send(200, "text/plain", "USERNAME_EXISTS");
          break;
        case User::CredentialsVerificationResult::ANY_USER_EXISTS:
          request->send(200, "text/plain", "ANY_USER_EXISTS");
          break;
        default:
          request->send(200, "text/plain", "UNKNOWN_ERROR");
      }

      return false;
    }
  }

  request->send(400);
  return false;
}

void Api::createUser(AsyncWebServerRequest* request) {
  sprintln("!createUser");
  //username, password - ověřit validitu
  //username to lower
  //Ověřit, zda uživatel už neexistuje
  //Nastavit cookie
}

void Api::changePassword(AsyncWebServerRequest* request) {
  sprintln("!changePassword");
  //oldPassword, password
  //Ověřit zda souhlasí staré heslo
  //Ověřit validitu nového hesla
  //Přenastavit cookies
}

void Api::login(AsyncWebServerRequest* request) {
  sprintln("!login");
  //username, password
  //username to lower
  //ověřit validitu username, ověřit hash hesla
  //nastavit cookies
}

void Api::getQueueCount(AsyncWebServerRequest* request) {
  sprintln("!getQueueCount");
}

void Api::getBillCount(AsyncWebServerRequest* request) {
  sprintln("!getBillCount");
  //Username získat z cookies
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
  sprintln("!logout");
  //Zrušit cookies
}