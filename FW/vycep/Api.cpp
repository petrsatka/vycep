/*
By Petr Satka
Licnese CC-BY
*/

//Api rozhraní pro komunikace s web klientem.
#include "Api.h"

Api::Api(User& user, Valve& valve, Settings& settings)
  : user(user), valve(valve), settings(settings) {
}

Api::~Api() {
}

constexpr const char Api::credentialsVerificationResultNames[CRED_VERIF_ERR_COUNT][CRED_VERIF_ERR_BUFFER_SIZE];

const char* Api::getCredentialsVerificationResultName(User::CredentialsVerificationResult res) {
  dprintln("getCredentialsVerificationResultName");
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

void Api::serveAuth(AsyncWebServerRequest* request, uint32_t permissionMask, bool revalidateCookie, ResponseGetterFunction responseGetter, ErrorResponseGetterFunction noPermissionsresponseGetter, ErrorResponseGetterFunction errorResponseGetter) {
  dprintln("serveAuth");
  AsyncWebServerResponse* response = NULL;
  char cookie[User::COOKIE_BUFFER_SIZE] = { 0 };
  if (extractCookie(request, AHUTH_COOKIE_NAME, cookie)) {
    char username[User::USERNAME_BUFFER_SIZE] = { 0 };
    uint32_t permissions = 0;
    char newCookie[User::COOKIE_BUFFER_SIZE] = { 0 };

    User::CookieVerificationResult res = this->user.getCookieInfo(cookie, revalidateCookie, username, &permissions, newCookie);
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

void Api::serveStaticAuth(AsyncWebServerRequest* request, const char* path, uint32_t permissionMask, bool revalidateCookie) {
  dprintln("serveStaticAuth");
  serveAuth(
    request, permissionMask, revalidateCookie, [request, path](const char* lCaseUsername, uint32_t& permissions, const char* cookie, char* newCookie, bool& setCookie) {
      return request->beginResponse(LittleFS, path);
    },
    NULL, [request]() {
      AsyncWebServerResponse* response = request->beginResponse(302);
      response->addHeader("Location", "/login.html");
      return response;
    });
}

void Api::serveDynamicAuth(AsyncWebServerRequest* request, ResponseGetterFunction responseGetter, uint32_t permissionMask, bool revalidateCookie) {
  dprintln("serveDynamicAuth");
  serveAuth(
    request, permissionMask, revalidateCookie, responseGetter, [request]() {
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
  dprintln("unsetCookies");
  bool res = unsetCookie(response, AHUTH_COOKIE_NAME);
  return unsetCookie(response, USERNAME_COOKIE_NAME) && res;
}

bool Api::unsetCookie(AsyncWebServerResponse* response, const char* name) {
  dprintln("unsetCookie");
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
      String username = String(pUname->value().c_str());
      username.trim();
      User::CredentialsVerificationResult res = user.registerFirstAdmin(username.c_str(), pPassword->value().c_str(), lCaseUsername);
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
      String username = String(pUname->value().c_str());
      username.trim();
      User::CredentialsVerificationResult res = user.registerUser(username.c_str(), pPassword->value().c_str(), lCaseUsername);
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
    User::PERMISSIONS_ACTIVE, false);
}

bool Api::login(AsyncWebServerRequest* request) {
  dprintln("login");
  if (request->hasParam("username", true)) {
    AsyncWebParameter* pUname = request->getParam("username", true);
    if (request->hasParam("password", true)) {
      AsyncWebParameter* pPassword = request->getParam("password", true);
      char lCaseUsername[User::USERNAME_BUFFER_SIZE] = { 0 };
      String username = String(pUname->value().c_str());
      username.trim();
      if (user.verifyPassword(username.c_str(), pPassword->value().c_str(), lCaseUsername) == User::CredentialsVerificationResult::OK) {
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
  dprintln("getQueueCount");
  serveDynamicAuth(
    request, [&](const char* lCaseUsername, uint32_t& permissions, const char* cookie, char* newCookie, bool& setCookie) {
      AsyncWebServerResponse* response = request->beginResponse(
        200,
        "text/plain",
        String(GENERAL_SUCCESS_RESULT_CODE) + "&" + String(valve.getQueueCount()));
      return response;
    },
    User::PERMISSIONS_NO_PERMISSIONS, false);
}

void Api::getUserBillCount(AsyncWebServerRequest* request) {
  dprintln("getUserBillCount");
  serveDynamicAuth(
    request, [&](const char* lCaseUsername, uint32_t& permissions, const char* cookie, char* newCookie, bool& setCookie) {
      if (request->hasParam("username", true)) {
        AsyncWebParameter* pUname = request->getParam("username", true);
        char lcUname[User::USERNAME_BUFFER_SIZE] = { 0 };
        Utils::toLowerStr(pUname->value().c_str(), lcUname, User::USERNAME_BUFFER_SIZE);
        AsyncWebServerResponse* response = request->beginResponse(
          200,
          "text/plain",
          String(GENERAL_SUCCESS_RESULT_CODE) + "&" + String(user.getUserBill(lcUname)));
        return response;
      }

      return request->beginResponse(400);
    },
    User::PERMISSIONS_ACTIVE | User::PERMISSIONS_ADMIN, false);
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
    User::PERMISSIONS_NO_PERMISSIONS, false);
}

void Api::makeOrder(AsyncWebServerRequest* request) {
  sprintln("!makeOrder");
  serveDynamicAuth(
    request, [&](const char* lCaseUsername, uint32_t& permissions, const char* cookie, char* newCookie, bool& setCookie) {
      const char* resultCode = GENERAL_ERROR_RESULT_CODE;
      uint16_t userBill = 0;
      if (valve.makeOrder()) {
        resultCode = GENERAL_SUCCESS_RESULT_CODE;
        user.addUserBill(lCaseUsername, valve.getMode() == Settings::DeviceMode::AUTO ? 1 : 0, userBill);
      } else {
        resultCode = UNABLE_TO_PLACE_ORDER_RESULT_CODE;
      }

      AsyncWebServerResponse* response = request->beginResponse(
        200,
        "text/plain",
        String(resultCode) + "&" + String(valve.getQueueCount()) + "&" + String(userBill));
      return response;
    },
    User::PERMISSIONS_ACTIVE, false);
}

const char* Api::doPayment(const char* lCaseUsername, const char* amount, uint16_t& userBill, int32_t& amountValue) {
  sprintln("!doPayment");
  const char* resultCode = GENERAL_ERROR_RESULT_CODE;
  amountValue = strtol(amount, nullptr, 10);
  userBill = user.getUserBill(lCaseUsername);
  if (amountValue > 0 && amountValue <= userBill) {
    amountValue = min(userBill, static_cast<uint16_t>(amountValue));
    resultCode = GENERAL_SUCCESS_RESULT_CODE;
    user.addUserBill(lCaseUsername, -amountValue, userBill);
  } else {
    amountValue = 0;
    resultCode = INVALID_AMOUNT_VALUE_RESULT_CODE;
  }

  return resultCode;
}

void Api::pay(AsyncWebServerRequest* request) {
  sprintln("!pay");
  serveDynamicAuth(
    request, [&](const char* lCaseUsername, uint32_t& permissions, const char* cookie, char* newCookie, bool& setCookie) {
      if (request->hasParam("amount", true)) {
        AsyncWebParameter* pAmount = request->getParam("amount", true);
        int32_t amountValue = 0;
        uint16_t userBill = 0;
        const char* resultCode = doPayment(lCaseUsername, pAmount->value().c_str(), userBill, amountValue);
        AsyncWebServerResponse* response = request->beginResponse(
          200,
          "text/plain",
          String(resultCode) + "&" + String(amountValue) + "&" + String(userBill));
        return response;
      }

      return request->beginResponse(400);
    },
    User::PERMISSIONS_ACTIVE | User::PERMISSIONS_PAYMENT, false);
}

void Api::payForUser(AsyncWebServerRequest* request) {
  sprintln("!payForUser");
  serveDynamicAuth(
    request, [&](const char* lCaseUsername, uint32_t& permissions, const char* cookie, char* newCookie, bool& setCookie) {
      if (request->hasParam("amount", true) && request->hasParam("username", true)) {
        AsyncWebParameter* pUname = request->getParam("username", true);
        char lcUname[User::USERNAME_BUFFER_SIZE] = { 0 };
        Utils::toLowerStr(pUname->value().c_str(), lcUname, User::USERNAME_BUFFER_SIZE);
        AsyncWebParameter* pAmount = request->getParam("amount", true);
        int32_t amountValue = 0;
        uint16_t userBill = 0;
        const char* resultCode = doPayment(lcUname, pAmount->value().c_str(), userBill, amountValue);
        AsyncWebServerResponse* response = request->beginResponse(
          200,
          "text/plain",
          String(resultCode) + "&" + String(amountValue) + "&" + String(userBill));
        return response;
      }

      return request->beginResponse(400);
    },
    User::PERMISSIONS_ACTIVE | User::PERMISSIONS_ADMIN | User::PERMISSIONS_PAYMENT, false);
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
    User::PERMISSIONS_ACTIVE | User::PERMISSIONS_ADMIN, false);
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
    User::PERMISSIONS_ACTIVE | User::PERMISSIONS_ADMIN, false);
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
    User::PERMISSIONS_ACTIVE | User::PERMISSIONS_ADMIN, false);
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
    User::PERMISSIONS_ACTIVE | User::PERMISSIONS_ADMIN, true);

  return res;
}

void Api::startCalibration(AsyncWebServerRequest* request) {
  sprintln("!startCalibration");
  serveDynamicAuth(
    request, [&](const char* lCaseUsername, uint32_t& permissions, const char* cookie, char* newCookie, bool& setCookie) {
      valve.startCalibration();
      AsyncWebServerResponse* response = request->beginResponse(
        200,
        "text/plain",
        String(GENERAL_SUCCESS_RESULT_CODE));
      return response;
    },
    User::PERMISSIONS_ACTIVE | User::PERMISSIONS_ADMIN, true);
}

void Api::stopCalibration(AsyncWebServerRequest* request) {
  sprintln("!stopCalibration");
  serveDynamicAuth(
    request, [&](const char* lCaseUsername, uint32_t& permissions, const char* cookie, char* newCookie, bool& setCookie) {
      int16_t pulseCount = valve.stopCalibration(settings.getMode());
      if (pulseCount > 0) {
        settings.setPulsePerServingCount(pulseCount);
         valve.configure(settings.getPulsePerServingCount(), Utils::FLOW_METER_PIN, Utils::VALVE_PIN, settings.getMasterTimeoutSeconds());
      }

      AsyncWebServerResponse* response = request->beginResponse(
        200,
        "text/plain",
        String(GENERAL_SUCCESS_RESULT_CODE));
      return response;
    },
    User::PERMISSIONS_ACTIVE | User::PERMISSIONS_ADMIN, true);
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
    User::PERMISSIONS_ACTIVE | User::PERMISSIONS_ADMIN, true);

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
            res = String(Utils::bToStr(settings.getNewUserPaymentEnabled()));
          } else if (strcmp(key, Settings::KEY_SSID) == 0) {
            char ssid[Utils::SSID_BUFFER_SIZE] = { 0 };
            settings.getSSID(ssid);
            res = String(ssid);
          } else if (strcmp(key, Settings::KEY_SECURITY_KEY) == 0) {
            char secKey[Utils::SECURITY_KEY_BUFFER_SIZE] = { 0 };
            settings.getSecurityKey(secKey);
            res = String(secKey);
          } else if (strcmp(key, Settings::KEY_PULSE_PER_SERVING) == 0) {
            res = String(settings.getPulsePerServingCount());
          } else if (strcmp(key, Settings::KEY_MODE) == 0) {
            res = String(getDeviceModeName(static_cast<Settings::DeviceMode>(settings.getMode())));
          } else if (strcmp(key, Settings::KEY_MASTER_TIMEOUT) == 0) {
            res = String(settings.getMasterTimeoutSeconds() / 60);
          } else if (strcmp(key, Settings::KEY_UNDER_LIMIT_TIMEOUT) == 0) {
            res = String(settings.getUnderLimitTimeoutSeconds() / 60);
          } else if (strcmp(key, Settings::KEY_DDNS_DOMAIN) == 0) {
            char ddnsDomain[Settings::DDNSS_DOAMIN_BUFFER_SIZE] = { 0 };
            settings.getDdnsDomain(ddnsDomain);
            res = String(ddnsDomain);
          } else if (strcmp(key, Settings::KEY_DDNS_USERNAME) == 0) {
            char ddnsUsername[Settings::DDNSS_USERNAME_BUFFER_SIZE] = { 0 };
            settings.getDdnsUsername(ddnsUsername);
            res = String(ddnsUsername);
          } else if (strcmp(key, Settings::KEY_MDNS_HOSTNAME) == 0) {
            char mdnsHostname[Settings::MDNS_HOSTNAME_BUFFER_SIZE] = { 0 };
            settings.getMdnsHostname(mdnsHostname);
            res = String(mdnsHostname);
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
    User::PERMISSIONS_ACTIVE | User::PERMISSIONS_ADMIN, false);
}

void Api::getUsers(AsyncWebServerRequest* request) {
  dprintln("getUsers");
  serveDynamicAuth(
    request, [&](const char* lCaseUsername, uint32_t& permissions, const char* cookie, char* newCookie, bool& setCookie) {
      const char* resultCode = GENERAL_SUCCESS_RESULT_CODE;
      AsyncResponseStream* response = request->beginResponseStream("text/plain");
      unsigned short userCount = user.getUserCount();
      char** usernames = new char*[userCount]();
      response->printf("%s&", resultCode);
      response->printf("{\"paymentEnabled\": %s,", Utils::bToStr(User::checkPermissions(permissions, User::PERMISSIONS_PAYMENT)));
      response->print("\"users\":[");
      user.iterateUsers([usernames, userCount](const char* key, unsigned short index) {
        if (index < userCount) {
          char* unameBuffer = new char[User::USERNAME_BUFFER_SIZE]();
          strcpy(unameBuffer, key);
          usernames[index] = unameBuffer;
        }
      });

      for (int i = 0; i < userCount; i++) {
        uint32_t permissions = user.getPermissions(usernames[i]);
        if (i) {
          response->print(",");
        }

        response->printf("{ \"username\": \"%s\", \"payment\": %s, \"admin\": %s, \"active\": %s }",
                         usernames[i],
                         Utils::bToStr(User::checkPermissions(permissions, User::PERMISSIONS_PAYMENT)),
                         Utils::bToStr(User::checkPermissions(permissions, User::PERMISSIONS_ADMIN)),
                         Utils::bToStr(User::checkPermissions(permissions, User::PERMISSIONS_ACTIVE)));
        delete (usernames[i]);
        usernames[i] = NULL;
      }

      response->print("]}");
      delete (usernames);
      usernames = NULL;
      return response;
    },
    User::PERMISSIONS_ACTIVE | User::PERMISSIONS_ADMIN, false);
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
            settings.setNewUserPaymentEnabled(Utils::strTob(value));
          } else if (strcmp(key, Settings::KEY_PULSE_PER_SERVING) == 0) {
            settings.setPulsePerServingCount((unsigned int)strtoul(value, nullptr, 10));
            valve.configure(settings.getPulsePerServingCount(), Utils::FLOW_METER_PIN, Utils::VALVE_PIN, settings.getMasterTimeoutSeconds());
          } else if (strcmp(key, Settings::KEY_MODE) == 0) {
            Settings::DeviceMode mode = getDeviceModeByName(value);
            settings.setMode(mode);
            valve.setMode(mode);
          } else if (strcmp(key, Settings::KEY_MASTER_TIMEOUT) == 0) {
            settings.setMasterTimeoutSeconds(strtoul(value, nullptr, 10) * 60);
            valve.setMasterTimeout(settings.getMasterTimeoutSeconds());
          } else if (strcmp(key, Settings::KEY_UNDER_LIMIT_TIMEOUT) == 0) {
            settings.setUnderLimitTimeoutSeconds(strtoul(value, nullptr, 10) * 60);
          } else if (strcmp(key, Settings::KEY_DDNS_DOMAIN) == 0) {
            if (!settings.setDdnsDomain(value)) {
              resultCode = GENERAL_ERROR_RESULT_CODE;
            }
          } else if (strcmp(key, Settings::KEY_DDNS_USERNAME) == 0) {
            if (!settings.setDdnsUsername(value)) {
              resultCode = GENERAL_ERROR_RESULT_CODE;
            }
          } else if (strcmp(key, Settings::KEY_DDNS_PASSWORD) == 0) {
            if (!settings.setDdnsPassword(value)) {
              resultCode = GENERAL_ERROR_RESULT_CODE;
            }
          } else if (strcmp(key, Settings::KEY_MDNS_HOSTNAME) == 0) {
            if (!settings.setMdnsHostname(value)) {
              resultCode = GENERAL_ERROR_RESULT_CODE;
            }
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
    User::PERMISSIONS_ACTIVE | User::PERMISSIONS_ADMIN, true);
}

void Api::setPermissionsValue(AsyncWebServerRequest* request) {
  dprintln("setPermissionsValue");
  serveDynamicAuth(
    request, [&](const char* lCaseUsername, uint32_t& permissions, const char* cookie, char* newCookie, bool& setCookie) {
      if (request->hasParam("username", true) && request->hasParam("key", true) && request->hasParam("value", true)) {
        AsyncWebParameter* pUsername = request->getParam("username", true);
        AsyncWebParameter* pKey = request->getParam("key", true);
        AsyncWebParameter* pValue = request->getParam("value", true);
        const char* resultCode = GENERAL_ERROR_RESULT_CODE;
        const char* username = pUsername->value().c_str();
        const char* key = pKey->value().c_str();
        const char* value = pValue->value().c_str();
        String res = "";
        if (key != NULL && key[0] != 0 && username != NULL && username[0] != 0 && value != NULL) {
          if (strcmp(username, lCaseUsername) != 0) {
            if (strcmp(key, Settings::KEY_ADMIN_PERMISSIONS) == 0) {
              if (Utils::strTob(value)) {
                if (user.addPermissions(username, User::PERMISSIONS_ADMIN)) {
                  resultCode = GENERAL_SUCCESS_RESULT_CODE;
                }
              } else {
                if (user.removePermissions(username, User::PERMISSIONS_ADMIN)) {
                  resultCode = GENERAL_SUCCESS_RESULT_CODE;
                }
              }
            } else if (strcmp(key, Settings::KEY_PAYMENT_PERMISSIONS) == 0) {
              if (Utils::strTob(value)) {
                if (user.addPermissions(username, User::PERMISSIONS_PAYMENT)) {
                  resultCode = GENERAL_SUCCESS_RESULT_CODE;
                }
              } else {
                if (user.removePermissions(username, User::PERMISSIONS_PAYMENT)) {
                  resultCode = GENERAL_SUCCESS_RESULT_CODE;
                }
              }
            } else {
              resultCode = INVALID_KEY_RESULT_CODE;
            }
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
    User::PERMISSIONS_ACTIVE | User::PERMISSIONS_ADMIN, true);
}

void Api::activateUser(AsyncWebServerRequest* request) {
  dprintln("activateUser");
  serveDynamicAuth(
    request, [&](const char* lCaseUsername, uint32_t& permissions, const char* cookie, char* newCookie, bool& setCookie) {
      if (request->hasParam("username", true)) {
        AsyncWebParameter* pUsername = request->getParam("username", true);
        const char* resultCode = GENERAL_ERROR_RESULT_CODE;
        const char* username = pUsername->value().c_str();
        String res = "";
        if (username != NULL && username[0] != 0) {
          if (strcmp(username, lCaseUsername) != 0) {
            uint32_t perms = User::PERMISSIONS_ACTIVE;
            if (settings.getNewUserPaymentEnabled()) {
              perms = perms | User::PERMISSIONS_PAYMENT;
            }

            if (user.setPermissions(username, user.getPermissions(username) | perms)) {
              resultCode = GENERAL_SUCCESS_RESULT_CODE;
            }
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
    User::PERMISSIONS_ACTIVE | User::PERMISSIONS_ADMIN, true);
}

void Api::deleteUser(AsyncWebServerRequest* request) {
  sprintln("!deleteUser");
  serveDynamicAuth(
    request, [&](const char* lCaseUsername, uint32_t& permissions, const char* cookie, char* newCookie, bool& setCookie) {
      if (request->hasParam("username", true)) {
        AsyncWebParameter* pUsername = request->getParam("username", true);
        const char* resultCode = GENERAL_ERROR_RESULT_CODE;
        const char* username = pUsername->value().c_str();
        String res = "";
        if (username != NULL && username[0] != 0) {
          if (strcmp(username, lCaseUsername) != 0) {
            if (user.deleteUser(username)) {
              resultCode = GENERAL_SUCCESS_RESULT_CODE;
            }
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
    User::PERMISSIONS_ACTIVE | User::PERMISSIONS_ADMIN, true);
}

void Api::resetPassword(AsyncWebServerRequest* request) {
  dprintln("resetPassword");
  serveDynamicAuth(
    request, [&](const char* lCaseUsername, uint32_t& permissions, const char* cookie, char* newCookie, bool& setCookie) {
      if (request->hasParam("username", true)) {
        char newPassword[User::PASSWORD_MIN_CHAR_COUNT + 1] = { 0 };
        AsyncWebParameter* pUsername = request->getParam("username", true);
        const char* resultCode = GENERAL_ERROR_RESULT_CODE;
        const char* username = pUsername->value().c_str();
        String res = "";
        if (username != NULL && username[0] != 0) {
          if (user.resetPassword(username, newPassword, User::PASSWORD_MIN_CHAR_COUNT + 1)) {
            resultCode = GENERAL_SUCCESS_RESULT_CODE;
          }
        }

        AsyncWebServerResponse* response = request->beginResponse(
          200,
          "text/plain",
          String(resultCode) + "&" + String(newPassword));
        return response;
      }

      return request->beginResponse(400);
    },
    User::PERMISSIONS_ACTIVE | User::PERMISSIONS_ADMIN, true);
}

void Api::logout(AsyncWebServerRequest* request) {
  dprintln("logout");
  AsyncWebServerResponse* response = request->beginResponse(200, "text/plain", GENERAL_SUCCESS_RESULT_CODE);
  unsetCookies(response);
  request->send(response);
}