#include <Arduino.h>
#include <ESPAsyncWebSrv.h>
#include <LittleFS.h>
#include <time.h>
#include "User.h"

AsyncWebServer server(80);
SemaphoreHandle_t xSemaphore = xSemaphoreCreateMutex();
User user(xSemaphore);

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
  server.serveStatic("/login.html", LittleFS, "/www/login.html");

  ///TEST
  server.on("/api/test", HTTP_GET,
            [](AsyncWebServerRequest *request) {
              ///

              //
              AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "xxx");
              response->addHeader("Set-Cookie:", "testCookie=abcd");
              request->send(response);
            }).setFilter(filterNotLoggetIn);
  //    server.on("/api/getUser", HTTP_GET,
  //         [](AsyncWebServerRequest *request) {
  //           char displayName[15] = {'\0'};
  //           users.getUserCookie("aaa", displayName);
  //           request->send(200, "text/plain", displayName);
  //         })
  // .setFilter(filterNotLoggetIn);
  ///TEST

  //Chráněný obsah
  //html
  server.on("/*.html", HTTP_GET,
            [](AsyncWebServerRequest *request) {
              request->redirect("/login.html");
            })
    .setFilter(filterNotLoggetIn);
  //defaultDocument
  server.on("/", HTTP_GET,
            [](AsyncWebServerRequest *request) {
              request->redirect("/login.html");
            })
    .setFilter(filterNotLoggetIn);
  //api
  server.on("/api/*", HTTP_GET,
            [](AsyncWebServerRequest *request) {
              request->send(401);
            })
    .setFilter(filterNotLoggetIn);

  //Sem ještě někde vrazit filtr podle oprávnění

  //Částečně chráněný obsah. Co nezaychtí handlery nahoře, projde sem.
  //Api
  server.on("/api/heap", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(ESP.getFreeHeap()));
  });

  //Statický obsah
  server.serveStatic("/", LittleFS, "/www/")
    .setDefaultFile("orders.html");

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
