#include <Arduino.h>
#include <ESPAsyncWebSrv.h>
#include <LittleFS.h>
#include <time.h>
#include "User.h"
#include "Api.h"

AsyncWebServer server(80);
SemaphoreHandle_t xSemaphore = xSemaphoreCreateMutex();
User user(xSemaphore);

AsyncCallbackWebHandler *firstRegistrationRedirectHandler = NULL;

void serverInit() {
  //Vždy přístupný obsah
  server.serveStatic("/images/", LittleFS, "/www/images/");
  server.serveStatic("/scripts/", LittleFS, "/www/scripts/");
  server.serveStatic("/styles/", LittleFS, "/www/styles/");
  server.serveStatic("/login.html", LittleFS, "/www/login.html");
  server.serveStatic("/registration.html", LittleFS, "/www/registration.html");

  //Pouze pokud nejsou uživatelé
  if (!user.isUserSet()) {
    firstRegistrationRedirectHandler = &server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->redirect("/first-registration.html");
    });

    //Po registraci odstranit handler
    //server.removeHandler(firstRegistrationRedirectHandler);
    //firstRegistrationRedirectHandler = NULL;
  }

  //Default webpage
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->redirect("/orders.html");
  });

  //Rozdělení obsahu podle práv
  server.on("/rolespecific/menu-content.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (Utils::isAuthorized(request, User::PERMISSIONS_ADMIN)) {
      request->send(LittleFS, "/www/rolespecific/menu-content-admin.js");
    } else {
      request->send(LittleFS, "/www/rolespecific/menu-content.js");
    }
  });

  //Vlastní podmínka místo filtrů - je to efektivnější
  //Přihlášený uživatel
  server.on("/orders.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    Utils::serveStaticAuth(request, "/www/orders.html", User::PERMISSIONS_ANY_PERMISSIONS);
  });

  server.on("/payment.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    Utils::serveStaticAuth(request, "/www/payment.html", User::PERMISSIONS_ANY_PERMISSIONS);
  });

  server.on("/password-change.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    Utils::serveStaticAuth(request, "/www/password-change.html", User::PERMISSIONS_ANY_PERMISSIONS);
  });

  //Admin
  server.on("/users.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    Utils::serveStaticAuth(request, "/www/users.html", User::PERMISSIONS_ADMIN);
  });

  server.on("/settings.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    Utils::serveStaticAuth(request, "/www/settings.html", User::PERMISSIONS_ADMIN);
  });

  //ZDE API - kontrouje se až uvnitř
  server.on("/api/test", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", String(ESP.getFreeHeap()));
    response->addHeader("Set-Cookie", "ESPAUTH=abcd; Path=/");
    request->send(response);
  });

  server.on("/first-registration.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (user.isUserSet()) {
      request->redirect("/login.html");
    } else {
      request->send(LittleFS, "/www/first-registration.html");
    }
  });

  //Fallback
  server.onNotFound(Utils::onNotFound);
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
  delay(60 * 1000);
}
