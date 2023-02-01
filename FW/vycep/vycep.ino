#include "ESPAsyncWebSrv.h"
#include "LittleFS.h"
AsyncWebServer server(80);

void onNotFound(AsyncWebServerRequest *request) {
  request->send(404);
}

bool filterNotLoggetIn(AsyncWebServerRequest *request) {
  return false;
}

void serverInit() {

  //Vždy přístupný obsah
  server.serveStatic("/login.html", LittleFS, "/www/login.html");
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
              request->redirect("/login.html");
            })
    .setFilter(filterNotLoggetIn);

  //Částečně chráněný obsah. Co nezaychtí handlery nahoře, projde sem.
  //Api
  server.on("/api/heap", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(ESP.getFreeHeap()));
  });

  //Statický obsah
  server.serveStatic("/", LittleFS, "/www/").setDefaultFile("orders.html");

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

  Serial.println(WiFi.localIP().toString());

  serverInit();
}

void loop() {
  // put your main code here, to run repeatedly:
}
