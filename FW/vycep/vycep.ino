#include <Arduino.h>
#include <ESPAsyncWebSrv.h>
#include <LittleFS.h>
#include <time.h>
#include "User.h"
#include "Api.h"
#include "Debug.h"
#include "Utils.h"

AsyncWebServer server(80);
//Mutex pro řízení přístupu k FLASH
SemaphoreHandle_t xSemaphore = xSemaphoreCreateMutex();
User user(xSemaphore);
Api api(user);

//Callback handler registrace prvního admina. Po vytvořaní admina bude odstraněn.
AsyncCallbackWebHandler *firstRegistrationRedirectHandler = NULL;

void test() {
  user.clearAll();
  user.test();
  Utils::test();
  user.clearAll();
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
      request, User::PERMISSIONS_ANY_PERMISSIONS, [request]() {
        AsyncWebServerResponse *response = request->beginResponse(302);
        response->addHeader("Location", "/orders.html");
        return response;
      },
      NULL, [request]() {
        request->send(LittleFS, "/www/login.html");
      });
  });

  //Pokud jde dotaz na registraci a uživatel je přilášen, pak přesměrovat na objednávky
  server.on("/registration.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    api.serveAuth(
      request, User::PERMISSIONS_ANY_PERMISSIONS, [request]() {
        AsyncWebServerResponse *response = request->beginResponse(302);
        response->addHeader("Location", "/orders.html");
        return response;
      },
      NULL, [request]() {
        request->send(LittleFS, "/www/registration.html");
      });
  });

  //Rozdělení obsahu podle práv
  server.on("/rolespecific/menu-content.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    api.serveAuth(
      request, User::PERMISSIONS_ADMIN, [request]() {
        return request->beginResponse(LittleFS, "/www/rolespecific/menu-content-admin.js");
      },
      [request]() {
        return request->beginResponse(LittleFS, "/www/rolespecific/menu-content.js");
      },
      [request]() {
        request->send(401);
      });
  });

  //Vlastní podmínka místo filtrů - je to efektivnější
  //Přihlášený uživatel
  server.on("/orders.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    api.serveStaticAuth(request, "/www/orders.html", User::PERMISSIONS_ANY_PERMISSIONS);
  });

  server.on("/payment.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    api.serveStaticAuth(request, "/www/payment.html", User::PERMISSIONS_ANY_PERMISSIONS);
  });

  server.on("/password-change.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    api.serveStaticAuth(request, "/www/password-change.html", User::PERMISSIONS_ANY_PERMISSIONS);
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
    dprintln("Call createUser");
    if (api.createUser(request)) {
      dprintln("user created");
    }
  });

  server.on("/api/changePassword", HTTP_POST, [](AsyncWebServerRequest *request) {
    api.changePassword(request);
  });

  server.on("/api/getBillCount", HTTP_POST, [](AsyncWebServerRequest *request) {
    api.getBillCount(request);
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
    api.login(request);
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

  WiFi.mode(WIFI_STA);
  WiFi.begin("PaPaJ 2.4", "petapajajola");
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    sprintln("WiFi Failed!");
    return;
  }

  struct tm timeInfo;
  if (!getLocalTime(&timeInfo, 10000)) {
    //čekání na NTP. max 10s;
    sprintln("NTP Timeout");
  }

  sprintln(WiFi.localIP().toString());
  user.clearAll(); //Debug - odstranit !!!!
  serverInit();
  sprintln("Start");
  test();  //Debug - odstranit !!!!!
}

void loop() {
  sprintln(ESP.getFreeHeap());
  delay(60 * 1000);
}
