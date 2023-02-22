#include <Arduino.h>
#include <ESPAsyncWebSrv.h>
#include <LittleFS.h>
#include <time.h>
#include "User.h"

AsyncWebServer server(80);
SemaphoreHandle_t xSemaphore = xSemaphoreCreateMutex();
User user(xSemaphore);

AsyncStaticWebHandler *firstRegistrationHandler = NULL;

void onNotFound(AsyncWebServerRequest *request) {
  request->send(404);
}

bool filterNotLoggetIn(AsyncWebServerRequest *request) {
  int headers = request->headers();
  int i;
  for (i = 0; i < headers; i++) {
    AsyncWebHeader *h = request->getHeader(i);
    Serial.printf("HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
  }
  return true;
}

void serverInit() {

  //Vždy přístupný obsah
  server.serveStatic("/images/", LittleFS, "/www/images/");
  server.serveStatic("/scripts/", LittleFS, "/www/scripts/");
  server.serveStatic("/styles/", LittleFS, "/www/styles/");
  server.serveStatic("/login.html", LittleFS, "/www/login.html");
  server.serveStatic("/registration.html", LittleFS, "/www/registration.html");

  //Pouze pokud nejsou uživatelé
  //DOPLNIT IF !!!
  firstRegistrationHandler = &server.serveStatic("/first-registration.html", LittleFS, "/www/first-registration.html");

  //Po registraci odstranit handler
  //server.removeHandler(firstRegistrationHandler);
  //firstRegistrationHandler = NULL;

  //Přihlášený uživatel
  server.serveStatic("/orders.html", LittleFS, "/www/orders.html");
  server.serveStatic("/password-change.html", LittleFS, "/www/password-change.html");

  //Přihlášený s právy
  server.serveStatic("/paymen.html", LittleFS, "/www/paymen.html");

  //Admin
  server.serveStatic("/users.html", LittleFS, "/www/users.html");
  server.serveStatic("/settings.html", LittleFS, "/www/settings.html");

  //Při pokusu o přístup na HTML, na které nemám práva
  server.on("/*.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->redirect("/login.html");
  });

  //Default webpage
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->redirect("/orders.html");
  });

  //ZDE API - rozhodnout se, zda aplikovat filtry, nebo kontrolovat až uvnitř
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
  configTime(3600, 3600, "pool.ntp.org");
  if (!LittleFS.begin()) {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin("PaPaJ 2.4", "petapajajola");
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!\n");
    return;
  }

  //čekání na NTP. max 5s;
  struct tm timeInfo;
  if (!getLocalTime(&timeInfo)) {
    Serial.println("NTP Timeout");
  }

  Serial.println(WiFi.localIP().toString());
  serverInit();
  Serial.println("Start");
}

void loop() {
  Serial.println(ESP.getFreeHeap());
  //time_t current = time(nullptr);
  //Serial.print(ctime(&current));
  delay(5000);
}
