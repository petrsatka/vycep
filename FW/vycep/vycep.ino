#include <Arduino.h>
#include <ESPAsyncWebSrv.h>
#include <LittleFS.h>
#include <time.h>
#include "User.h"
#include "Api.h"
#include "Debug.h"
#include "Utils.h"
#include "Settings.h"

static constexpr const char *NVS_PARTTION = "nvs_ext";

bool needReconnect = false;
unsigned long lastHeapPrintMillis = 0;
unsigned long lastTryReconnectMillis = 0;
const unsigned long heapPrintPeriod = 20000;
uint32_t heapAfterInit = 0;
bool shouldReboot = false;
AsyncWebServer server(80);
//Mutex pro řízení přístupu k FLASH
SemaphoreHandle_t xSemaphore = xSemaphoreCreateMutex();
Settings settings(xSemaphore, NVS_PARTTION);
User user(xSemaphore, NVS_PARTTION);
Api api(user, settings);

//Callback handler registrace prvního admina. Po vytvořaní admina bude odstraněn.
AsyncCallbackWebHandler *firstRegistrationRedirectHandler = NULL;

void test() {
  user.clearAll();
  user.test();
  Utils::test();
  user.clearAll();
}

void createAP() {
  dprintln("Create soft AP");
  IPAddress apIP = IPAddress(192, 168, 1, 1);
  IPAddress apGatewayIP = IPAddress(192, 168, 1, 1);
  IPAddress apSubnet = IPAddress(255, 255, 255, 0);

  WiFi.disconnect();
  dprintln("soft AP config");
  dprintln(WiFi.softAPConfig(apIP, apGatewayIP, apSubnet) ? "Ready" : "Failed!");
  String mac = WiFi.macAddress();
  mac.replace(":", "");
  String apName = "Vycep_" + mac;
  dprintln(WiFi.softAP(apName) ? "Ready" : "Failed!");
  dprintln(WiFi.softAPIP().toString());
}

bool connectWiFiClient() {
  char ssid[Utils::SSID_BUFFER_SIZE] = { 0 };
  char secKey[Utils::SECURITY_KEY_BUFFER_SIZE] = { 0 };
  dprintln("connectWiFiClient");
  WiFi.disconnect();
  dprintln("GetSSID");
  settings.getSSID(ssid);
  if (ssid[0] == 0) {
    dprintln("NoSSID");
    return false;
  }

  dprintln(ssid);
  dprintln("GetSecurityKey");
  settings.getSecurityKey(secKey);
  WiFi.begin(ssid, secKey);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    dprintln("ClientConnectionFailed");
    return false;
  }

  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  sprintln(WiFi.localIP().toString());
  return true;
}

void wiFiInit() {
  settings.clearAll();  //DEBUG odstranit!!!
  WiFi.mode(WIFI_STA);
  if (!connectWiFiClient()) {
    WiFi.mode(WIFI_AP_STA);
    createAP();
    needReconnect = true;
  }
}

//Inicalizace serveru
void serverInit() {
  dprintln("serverInit");
  //Vždy přístupný obsah
  server.serveStatic("/images/", LittleFS, "/www/images/");
  server.serveStatic("/scripts/", LittleFS, "/www/scripts/");
  server.serveStatic("/styles/", LittleFS, "/www/styles/");

  //Pouze pokud nejsou uživatelé
  if (!user.isAnyUserSet()) {
    firstRegistrationRedirectHandler = &server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->redirect("/first-registration.html");
    });
  }

  //Default webpage
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->redirect("/orders.html");
  });

  //Pokud jde dotaz na login a uživatel je přilášen, pak přesměrovat na objednávky
  server.on("/login.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    api.serveAuth(
      request, User::PERMISSIONS_ANY_PERMISSIONS, [request](const char *lCaseUsername, uint32_t &permissions, const char *cookie, char *newCookie, bool &setCookie) {
        AsyncWebServerResponse *response = request->beginResponse(302);
        response->addHeader("Location", "/orders.html");
        return response;
      },
      NULL, [request]() {
        return request->beginResponse(LittleFS, "/www/login.html");
      });
  });

  //Pokud jde dotaz na registraci a uživatel je přilášen, pak přesměrovat na objednávky
  server.on("/registration.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    api.serveAuth(
      request, User::PERMISSIONS_ANY_PERMISSIONS, [request](const char *lCaseUsername, uint32_t &permissions, const char *cookie, char *newCookie, bool &setCookie) {
        AsyncWebServerResponse *response = request->beginResponse(302);
        response->addHeader("Location", "/orders.html");
        return response;
      },
      NULL, [request]() {
        return request->beginResponse(LittleFS, "/www/registration.html");
      });
  });

  //Rozdělení obsahu podle práv
  server.on("/rolespecific/menu-content.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    sprintln("!menu-content");
    api.serveAuth(
      request, User::PERMISSIONS_ANY_PERMISSIONS, [request](const char *lCaseUsername, uint32_t &permissions, const char *cookie, char *newCookie, bool &setCookie) {
        if (User::checkPermissions(permissions, User::PERMISSIONS_ACTIVE | User::PERMISSIONS_ADMIN | User::PERMISSIONS_PAYMENT)) {
          return request->beginResponse(LittleFS, "/www/rolespecific/menu-content-admin-payment.js");
        } else if (User::checkPermissions(permissions, User::PERMISSIONS_ACTIVE | User::PERMISSIONS_ADMIN)) {
          return request->beginResponse(LittleFS, "/www/rolespecific/menu-content-admin.js");
        } else if (User::checkPermissions(permissions, User::PERMISSIONS_ACTIVE | User::PERMISSIONS_PAYMENT)) {
          return request->beginResponse(LittleFS, "/www/rolespecific/menu-content-user-payment.js");
        } else if (User::checkPermissions(permissions, User::PERMISSIONS_ACTIVE)) {
          return request->beginResponse(LittleFS, "/www/rolespecific/menu-content-user.js");
        } else {
          return request->beginResponse(LittleFS, "/www/rolespecific/menu-content-inactive.js");
        }
      },
      NULL, NULL);
  });

  //Vlastní podmínka místo filtrů - je to efektivnější
  //Přihlášený uživatel
  server.on("/orders.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    api.serveStaticAuth(request, "/www/orders.html", User::PERMISSIONS_ANY_PERMISSIONS);
  });

  server.on("/payment.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    api.serveStaticAuth(request, "/www/payment.html", User::PERMISSIONS_ACTIVE | User::PERMISSIONS_PAYMENT);
  });

  server.on("/password-change.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    api.serveStaticAuth(request, "/www/password-change.html", User::PERMISSIONS_ACTIVE);
  });

  //Admin
  server.on("/users.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    api.serveStaticAuth(request, "/www/users.html", User::PERMISSIONS_ADMIN);
  });

  server.on("/settings.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    api.serveStaticAuth(request, "/www/settings.html", User::PERMISSIONS_ADMIN);
  });

  //API - práva se kontroují až uvnitř
  // server.on("/api/test", HTTP_GET, [](AsyncWebServerRequest *request) {
  //   test();
  //   request->send(200, "text/plain", "Test");
  // });

  server.on("/api/createFirstAdmin", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (api.createFirstAdmin(request)) {
      server.removeHandler(firstRegistrationRedirectHandler);
      firstRegistrationRedirectHandler = NULL;
      dprintln("First admin created");
    }
  });

  server.on("/api/createUser", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (api.createUser(request)) {
      dprintln("User created");
    }
  });

  server.on("/api/changePassword", HTTP_POST, [](AsyncWebServerRequest *request) {
    api.changePassword(request);
  });

  server.on("/api/getCurrentUserBillCount", HTTP_POST, [](AsyncWebServerRequest *request) {
    api.getCurrentUserBillCount(request);
  });

  server.on("/api/getQueueCount", HTTP_POST, [](AsyncWebServerRequest *request) {
    api.getQueueCount(request);
  });

  server.on("/api/getUserBillCount", HTTP_POST, [](AsyncWebServerRequest *request) {
    api.getUserBillCount(request);
  });

  server.on("/api/loadUsers", HTTP_POST, [](AsyncWebServerRequest *request) {
    api.loadUsers(request);
  });

  server.on("/api/login", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (api.login(request)) {
      dprintln("Logged in");
    }
  });

  server.on("/api/logout", HTTP_POST, [](AsyncWebServerRequest *request) {
    api.logout(request);
  });

  server.on("/api/pay", HTTP_POST, [](AsyncWebServerRequest *request) {
    api.pay(request);
  });

  server.on("/api/payForUser", HTTP_POST, [](AsyncWebServerRequest *request) {
    api.payForUser(request);
  });

  server.on("/api/makeOrder", HTTP_POST, [](AsyncWebServerRequest *request) {
    api.makeOrder(request);
  });

  server.on("/api/getIP", HTTP_POST, [](AsyncWebServerRequest *request) {
    api.getIP(request);
  });

  server.on("/api/getGatewayIP", HTTP_POST, [](AsyncWebServerRequest *request) {
    api.getGatewayIP(request);
  });

  server.on("/api/restart", HTTP_POST, [](AsyncWebServerRequest *request) {
    shouldReboot = api.restart(request);
  });

  server.on("/api/setWifiConnection", HTTP_POST, [](AsyncWebServerRequest *request) {
    shouldReboot = api.setWifiConnection(request);
  });

  server.on("/first-registration.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (user.isAnyUserSet()) {
      request->redirect("/login.html");
    } else {
      request->send(LittleFS, "/www/first-registration.html");
    }
  });

  //Fallback 404
  server.onNotFound(Api::onNotFound);
  //Hlavičky
  DefaultHeaders::Instance().addHeader("Cache-Control", "max-age=0, no-cache, no-store, must-revalidate");
  DefaultHeaders::Instance().addHeader("Pragma", "no-cache");
  DefaultHeaders::Instance().addHeader("Expires", "Wed, 11 Jan 1984 05:00:00 GMT");
  server.begin();
}

void setup() {
  Serial.begin(115200);

  //NTP
  configTime(3600, 3600, "pool.ntp.org");

  if (!LittleFS.begin()) {
    sprintln("LittleFS Failed");
    return;
  }

  wiFiInit();

  struct tm timeInfo;
  if (!getLocalTime(&timeInfo, 10000)) {
    //čekání na NTP. max 10s;
    sprintln("NTP Timeout");
  }

  //user.clearAll(); //Debug - odstranit !!!!
  serverInit();
  sprintln("Start");
  //test();  //Debug - odstranit !!!!!
  heapAfterInit = ESP.getFreeHeap();
}

void loop() {
  if (shouldReboot) {
    Serial.println("Rebooting...");
    delay(200);
    ESP.restart();
  }

  unsigned currentMillis = millis();
  if (currentMillis - lastHeapPrintMillis >= heapPrintPeriod) {
    sprintln((int64_t)ESP.getFreeHeap() - (int64_t)heapAfterInit);
    lastHeapPrintMillis = currentMillis;
  }

  if (needReconnect && currentMillis - lastTryReconnectMillis >= heapPrintPeriod) {
    dprintln("tryReconnect");
    // if (connectWiFiClient()) {
    //   ESP.restart();
    // } else {
    //    WiFi.disconnect(true);
    // }


    lastTryReconnectMillis = currentMillis;
  }
}
