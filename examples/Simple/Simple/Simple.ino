#include <ESP8266WebServer.h>
#include <WebOTA.h>

ESP8266WebServer server(80);

const char *ssid = "OTATEST";
const char *password = "12345679";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  if (!WiFi.softAP(ssid, password)) {
    Serial.println("Failed to initialise WiFi");
  }

  webota.init(&server, "/webota");

  server.begin();

  
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
}
