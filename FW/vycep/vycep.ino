#include <Arduino.h>
#include <ESPAsyncWebSrv.h>
#include <LittleFS.h>
#include <time.h>
#include "User.h"

AsyncWebServer server(80);
SemaphoreHandle_t xSemaphore = xSemaphoreCreateMutex();
User user(xSemaphore);

AsyncCallbackWebHandler *firstRegistrationRedirectHandler = NULL;
AsyncStaticWebHandler *firstRegistrationStaticHandler = NULL;

void onNotFound(AsyncWebServerRequest *request) {
  request->send(404);
}

bool isAuthenticatedFilter(AsyncWebServerRequest *request) {
  Serial.println("IsAuthenticatedFilter");
  return true;
}

bool isAuthorizedAdminFilter(AsyncWebServerRequest *request) {
  Serial.println("IsAdminFilter");
  return false;
}

void serverInit() {
  //Vždy přístupný obsah
  server.serveStatic("/images/", LittleFS, "/www/images/");
  server.serveStatic("/scripts/", LittleFS, "/www/scripts/");
  server.serveStatic("/styles/", LittleFS, "/www/styles/");
  server.serveStatic("/login.html", LittleFS, "/www/login.html");
  server.serveStatic("/registration.html", LittleFS, "/www/registration.html");

  //Pouze pokud nejsou uživatelé
  if (!user.isUserSet()) {
    // firstRegistrationRedirectHandler = &server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    //   request->redirect("/first-registration.html");
    // });

    // firstRegistrationStaticHandler = &server.serveStatic("/first-registration.html", LittleFS, "/www/first-registration.html");

    //Po registraci odstranit handler
    //server.removeHandler(firstRegistrationRedirectHandler);
    //firstRegistrationRedirectHandler = NULL;
    //server.removeHandler(firstRegistrationStaticHandler);
    //firstRegistrationStaticHandler = NULL;
  }

  //Default webpage
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->redirect("/orders.html");
  });

  //Rozdělení obsahu podle práv
  server.on("/rolespecific/menu-content.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (isAuthenticatedFilter(request) && isAuthorizedAdminFilter(request)) {
      request->send(LittleFS, "/www/rolespecific/menu-content-admin.js");
    } else {
      request->send(LittleFS, "/www/rolespecific/menu-content.js");
    }
  });

  //Přihlášený uživatel
  server.serveStatic("/orders.html", LittleFS, "/www/orders.html").setFilter(isAuthenticatedFilter);
  server.serveStatic("/password-change.html", LittleFS, "/www/password-change.html").setFilter(isAuthenticatedFilter);

  //Přihlášený s právy
  server.serveStatic("/payment.html", LittleFS, "/www/payment.html").setFilter(isAuthenticatedFilter);

  //Admin
  server.serveStatic("/users.html", LittleFS, "/www/users.html").setFilter(isAuthenticatedFilter).setFilter(isAuthorizedAdminFilter);
  server.serveStatic("/settings.html", LittleFS, "/www/settings.html").setFilter(isAuthenticatedFilter).setFilter(isAuthorizedAdminFilter);

  //Při pokusu o přístup na HTML, na které nemám práva
  server.on("/*.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->redirect("/login.html");
  });

  //OMEZIT FILTRY - kontrola až uvnitř requestu a pak buď odpověď nebo redirect na login

  //ZDE API - kontrouje se až uvnitř
  // server.on("/api/heap", HTTP_GET, [](AsyncWebServerRequest *request) {
  //   request->send(200, "text/plain", String(ESP.getFreeHeap()));
  // });

  //Ověřit, zda se filtry aplikují až na konec. Tedy že se nebudou zbytečně vyhodnocovat několikrát.

  //Fallback
  server.onNotFound(onNotFound);
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
    Serial.println("LittleFS Failed");
    return;
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin("PaPaJ 2.4", "petapajajola");
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }

  struct tm timeInfo;
  if (!getLocalTime(&timeInfo)) {
    //čekání na NTP. max 5s;
    Serial.println("NTP Timeout");
  }

  Serial.println(WiFi.localIP().toString());
  serverInit();
  Serial.println("Start");
}

void loop() {
  Serial.println(ESP.getFreeHeap());
  delay(60 * 10000);
}
