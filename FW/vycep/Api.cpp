#include "Api.h"

Api::Api(User& user, Settings& settings)
  : user(user), settings(settings) {
}

Api::~Api() {
}

constexpr const char Api::credentialsVerificationResultNames[CRED_VERIF_ERR_COUNT][CRED_VERIF_ERR_BUFFER_SIZE];

const char* Api::getCredentialsVerificationResultName(User::CredentialsVerificationResult res) {
  sprintln("!getCredentialsVerificationResultName");
  if (static_cast<int>(res) < 0 || static_cast<int>(res) >= CRED_VERIF_ERR_COUNT) {
    return Api::credentialsVerificationResultNames[static_cast<int>(User::CredentialsVerificationResult::UNKNOWN_ERROR)];
  }

  return Api::credentialsVerificationResultNames[static_cast<int>(res)];
}

constexpr const char Api::deviceModeNames[DEV_MODE_COUNT][DEV_MODE_BUFFER_SIZE];

const char* Api::getDeviceModeName(Settings::DeviceMode mode) {
  dprintln("getDeviceModeName");
  if (static_cast<int>(mode) < 0 || static_cast<int>(mode) >= DEV_MODE_COUNT) {
    return Api::deviceModeNames[static_cast<int>(Settings::DeviceMode::AUTO)];
  }

  return Api::deviceModeNames[static_cast<int>(mode)];
}

Settings::DeviceMode Api::getDeviceModeByName(const char* modeName) {
  dprintln("getDeviceModeByName");
  for (int i = 0; i < DEV_MODE_COUNT; i++) {
    if (strcmp(Api::deviceModeNames[i], modeName) == 0) {
      return static_cast<Settings::DeviceMode>(i);
    }
  }

  return Settings::DeviceMode::AUTO;
}

void Api::serveAuth(AsyncWebServerRequest* request, uint32_t permissionMask, ResponseGetterFunction responseGetter, ErrorResponseGetterFunction noPermissionsresponseGetter, ErrorResponseGetterFunction errorResponseGetter) {
  dprintln("serveAuth");
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
          response = responseGetter(username, permissions, cookie, newCookie, forceSetCookie);
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
    request, permissionMask, [request, path](const char* lCaseUsername, uint32_t& permissions, const char* cookie, char* newCookie, bool& setCookie) {
      return request->beginResponse(LittleFS, path);
    },
    NULL, [request]() {
      AsyncWebServerResponse* response = request->beginResponse(302);
      response->addHeader("Location", "/login.html");
      return response;
    });
}

void Api::serveDynamicAuth(AsyncWebServerRequest* request, ResponseGetterFunction responseGetter, uint32_t permissionMask) {
  dprintln("serveDynamicAuth");
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
  dprintln("changePassword");
  serveDynamicAuth(
    request, [&](const char* lCaseUsername, uint32_t& permissions, const char* cookie, char* newCookie, bool& setCookie) {
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

bool Api::login(AsyncWebServerRequest* request) {
  dprintln("login");
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
  dprintln("getUserBillCount");
  serveDynamicAuth(
    request, [&](const char* lCaseUsername, uint32_t& permissions, const char* cookie, char* newCookie, bool& setCookie) {
      if (request->hasParam("username", true)) {
        AsyncWebParameter* pUname = request->getParam("username", true);
        char lCaseUsername[User::USERNAME_BUFFER_SIZE] = { 0 };
        Utils::toLowerStr(pUname->value().c_str(), lCaseUsername, User::USERNAME_BUFFER_SIZE);
        AsyncWebServerResponse* response = request->beginResponse(
          200,
          "text/plain",
          String(GENERAL_SUCCESS_RESULT_CODE) + "&" + String(user.getUserBill(lCaseUsername)));
        return response;
      }

      return request->beginResponse(400);
    },
    User::PERMISSIONS_ADMIN);
}

void Api::getCurrentUserBillCount(AsyncWebServerRequest* request) {
  dprintln("getCurrentUserBillCount");
  serveDynamicAuth(
    request, [&](const char* lCaseUsername, uint32_t& permissions, const char* cookie, char* newCookie, bool& setCookie) {
      AsyncWebServerResponse* response = request->beginResponse(
        200,
        "text/plain",
        String(GENERAL_SUCCESS_RESULT_CODE) + "&" + String(user.getUserBill(lCaseUsername)));
      return response;
    },
    User::PERMISSIONS_ANY_PERMISSIONS);
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

void Api::getIP(AsyncWebServerRequest* request) {
  dprintln("getIP");
  serveDynamicAuth(
    request, [&](const char* lCaseUsername, uint32_t& permissions, const char* cookie, char* newCookie, bool& setCookie) {
      AsyncWebServerResponse* response = request->beginResponse(
        200,
        "text/plain",
        String(GENERAL_SUCCESS_RESULT_CODE) + "&" + WiFi.localIP().toString());
      return response;
    },
    User::PERMISSIONS_ADMIN);
}

void Api::getMAC(AsyncWebServerRequest* request) {
  dprintln("getMAC");
  serveDynamicAuth(
    request, [&](const char* lCaseUsername, uint32_t& permissions, const char* cookie, char* newCookie, bool& setCookie) {
      AsyncWebServerResponse* response = request->beginResponse(
        200,
        "text/plain",
        String(GENERAL_SUCCESS_RESULT_CODE) + "&" + WiFi.macAddress());
      return response;
    },
    User::PERMISSIONS_ADMIN);
}

void Api::getGatewayIP(AsyncWebServerRequest* request) {
  dprintln("getGatewayIP");
  serveDynamicAuth(
    request, [&](const char* lCaseUsername, uint32_t& permissions, const char* cookie, char* newCookie, bool& setCookie) {
      AsyncWebServerResponse* response = request->beginResponse(
        200,
        "text/plain",
        String(GENERAL_SUCCESS_RESULT_CODE) + "&" + WiFi.gatewayIP().toString());
      return response;
    },
    User::PERMISSIONS_ADMIN);
}

bool Api::restart(AsyncWebServerRequest* request) {
  dprintln("restart");
  bool res = false;
  serveDynamicAuth(
    request, [&](const char* lCaseUsername, uint32_t& permissions, const char* cookie, char* newCookie, bool& setCookie) {
      AsyncWebServerResponse* response = request->beginResponse(
        200,
        "text/plain",
        String(GENERAL_SUCCESS_RESULT_CODE));
      res = true;
      return response;
    },
    User::PERMISSIONS_ADMIN);

  return res;
}

bool Api::setWifiConnection(AsyncWebServerRequest* request) {
  bool res = false;
  dprintln("setWifiConnection");
  serveDynamicAuth(
    request, [&](const char* lCaseUsername, uint32_t& permissions, const char* cookie, char* newCookie, bool& setCookie) {
      if (request->hasParam("ssid", true) && request->hasParam("securitykey", true)) {
        AsyncWebParameter* pSSID = request->getParam("ssid", true);
        AsyncWebParameter* pSKey = request->getParam("securitykey", true);
        const char* resultCode = GENERAL_ERROR_RESULT_CODE;
        if (strlen(pSSID->value().c_str()) >= Utils::SSID_BUFFER_SIZE) {
          resultCode = SSID_TOO_LONG_RESULT_CODE;
        } else if (strlen(pSKey->value().c_str()) >= Utils::SECURITY_KEY_BUFFER_SIZE) {
          resultCode = SESURITY_KEY_TOO_LONG_RESULT_CODE;
        } else {
          settings.clearWiFiOK();
          if (settings.setSSID(pSSID->value().c_str()) && settings.setSecurityKey(pSKey->value().c_str())) {
            resultCode = GENERAL_SUCCESS_RESULT_CODE;
            res = true;
          }
        }

        AsyncWebServerResponse* response = request->beginResponse(
          200,
          "text/plain",
          String(resultCode));
        return response;
      }

      return request->beginResponse(400);
    },
    User::PERMISSIONS_ADMIN);

  return res;
}

void Api::getSettingsValue(AsyncWebServerRequest* request) {
  dprintln("getSettingsValue");
  serveDynamicAuth(
    request, [&](const char* lCaseUsername, uint32_t& permissions, const char* cookie, char* newCookie, bool& setCookie) {
      if (request->hasParam("key", true)) {
        AsyncWebParameter* pSKey = request->getParam("key", true);
        const char* resultCode = GENERAL_ERROR_RESULT_CODE;
        const char* key = pSKey->value().c_str();
        String res = "";
        if (key != NULL && key[0] != 0) {
          resultCode = GENERAL_SUCCESS_RESULT_CODE;
          if (strcmp(key, Settings::KEY_NEW_USER_PAYMNET) == 0) {
            res = String(settings.getNewUserPaymentEnabled() ? "true" : "false");
          } else if (strcmp(key, Settings::KEY_SSID) == 0) {
            char ssid[Utils::SSID_BUFFER_SIZE] = { 0 };
            settings.getSSID(ssid);
            res = String(ssid);
          } else if (strcmp(key, Settings::KEY_SECURITY_KEY) == 0) {
            char secKey[Utils::SECURITY_KEY_BUFFER_SIZE] = { 0 };
            settings.getSecurityKey(secKey);
            res = String(secKey);
          } else if (strcmp(key, Settings::KEY_PULSE_PER_LITER) == 0) {
            res = String(settings.getPulsePerLiterCount());
          } else if (strcmp(key, Settings::KEY_MODE) == 0) {
            res = String(getDeviceModeName(static_cast<Settings::DeviceMode>(settings.getMode())));
          } else if (strcmp(key, Settings::KEY_MASTER_TIMEOUT) == 0) {
            res = String(settings.getMasterTimeoutSeconds() / 60);
          } else if (strcmp(key, Settings::KEY_UNDER_LIMIT_TIMEOUT) == 0) {
            res = String(settings.getUnderLimitTimeoutSeconds() / 60);
          } else {
            resultCode = INVALID_KEY_RESULT_CODE;
          }
        }

        AsyncWebServerResponse* response = request->beginResponse(
          200,
          "text/plain",
          String(resultCode) + "&" + res);
        return response;
      }

      return request->beginResponse(400);
    },
    User::PERMISSIONS_ADMIN);
}

void Api::setSettingsValue(AsyncWebServerRequest* request) {
  dprintln("setSettingsValue");
  serveDynamicAuth(
    request, [&](const char* lCaseUsername, uint32_t& permissions, const char* cookie, char* newCookie, bool& setCookie) {
      if (request->hasParam("key", true) && request->hasParam("value", true)) {
        AsyncWebParameter* pKey = request->getParam("key", true);
        AsyncWebParameter* pValue = request->getParam("value", true);
        const char* resultCode = GENERAL_ERROR_RESULT_CODE;
        const char* key = pKey->value().c_str();
        const char* value = pValue->value().c_str();
        String res = "";
        if (key != NULL && key[0] != 0 && value != NULL) {
          resultCode = GENERAL_SUCCESS_RESULT_CODE;
          if (strcmp(key, Settings::KEY_NEW_USER_PAYMNET) == 0) {
            settings.setNewUserPaymentEnabled(strcmp("true", value) == 0);
          } else if (strcmp(key, Settings::KEY_PULSE_PER_LITER) == 0) {
            settings.setPulsePerLiterCount((unsigned int)strtoul(value, nullptr, 10));
          } else if (strcmp(key, Settings::KEY_MODE) == 0) {
            settings.setMode(getDeviceModeByName(value));
          } else if (strcmp(key, Settings::KEY_MASTER_TIMEOUT) == 0) {
            settings.setMasterTimeoutSeconds(strtoul(value, nullptr, 10) * 60);
          } else if (strcmp(key, Settings::KEY_UNDER_LIMIT_TIMEOUT) == 0) {
            settings.setUnderLimitTimeoutSeconds(strtoul(value, nullptr, 10) * 60);
          } else {
            resultCode = INVALID_KEY_RESULT_CODE;
          }
        }

        AsyncWebServerResponse* response = request->beginResponse(
          200,
          "text/plain",
          String(resultCode));
        return response;
      }

      return request->beginResponse(400);
    },
    User::PERMISSIONS_ADMIN);
}

void Api::logout(AsyncWebServerRequest* request) {
  dprintln("logout");
  AsyncWebServerResponse* response = request->beginResponse(200, "text/plain", GENERAL_SUCCESS_RESULT_CODE);
  unsetCookies(response);
  request->send(response);
}