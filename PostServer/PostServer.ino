#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "wifi-creds.h"
#include <ArduinoJson.h>


const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

#define zoomOutFast 13
#define zoomOutSlow 12
#define focus 14
#define power 16
#define zoomInFast 5
#define zoomInSlow 4

void handleJSON() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
  } else {

    JsonDocument doc;
    deserializeJson(doc, server.arg("plain"));
    for ( int i=0; i< doc.size() ; i++) {
      const char* action = doc[i]["action"];
      int duration       = doc[i]["duration"];

      /* Actions:
       *  - zoom(In|Out)(Fast|Slow);
       *  - power
       *  - focus
       */
      int pin;
      if (strcmp(action, "zoomInSlow") == 0 ) {
        Serial.println("zoomInSlow");
        handleAction(zoomInSlow,duration);
      } else if (strcmp(action, "zoomInFast") == 0 ) {
        Serial.println("zoomInFast");
        handleAction(zoomInFast,duration);
      } else if (strcmp(action, "zoomOutSlow") == 0 ) {
        Serial.println("zoomOutSlow");
        handleAction(zoomOutSlow,duration);
      } else if (strcmp(action, "zoomOutFast") == 0 ) {
        Serial.println("zoomOutFast");
        handleAction(zoomOutFast,duration);
      } else if (strcmp(action, "power") == 0 ) {
        Serial.println("power");
        handleAction(power,duration);
      } else if (strcmp(action, "focus") == 0 ) {
        Serial.println("focus");
        handleAction(focus,duration);
      } else {
        server.send(500, "text.plain", "argument not found: " + String(action));
      }
      // int pin
      // 4 + 0 = zoom in slow
      // 4 + 1 = zoom in fast
      // 12 + 0 = zoom out slow
      // 12 + 1 = zoom out fast
      server.send(200, "text.plain", String(action) + " executing for " + String(duration) + " mills.");
      handleAction(pin, duration);
    }

  }
}

void handleAction(int pin, int duration){  
  // Serial.print("setting pin " + String(pin) + " to low");
  digitalWrite(pin, LOW);

  if(pin == zoomOutFast) {
    digitalWrite(zoomOutSlow, LOW);
  }
  if(pin == zoomInFast) {
    digitalWrite(zoomInSlow, LOW);
  }

  delay(duration);
  digitalWrite(pin, HIGH);
  // Serial.println("setting pin " + String(pin) + " to high");
  
  if(pin == zoomOutFast) {
    digitalWrite(zoomOutSlow, HIGH);
  }
  if(pin == zoomInFast) {
    digitalWrite(zoomInSlow, HIGH);
  }
}

void setupPins(void) {
  pinMode(zoomOutFast, OUTPUT);
  pinMode(zoomOutSlow, OUTPUT);
  pinMode(focus, OUTPUT);
  pinMode(power, OUTPUT);
  pinMode(zoomInFast, OUTPUT);
  pinMode(zoomInSlow, OUTPUT);

  digitalWrite(zoomOutFast, 1);
  digitalWrite(zoomOutSlow, 1);
  digitalWrite(focus, 1);
  digitalWrite(power, 1);
  digitalWrite(zoomInFast, 1);
  digitalWrite(zoomInSlow, 1);
}


void setup(void) {
  setupPins();
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) { Serial.println("MDNS responder started"); }

  server.on("/pin", handleJSON);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}