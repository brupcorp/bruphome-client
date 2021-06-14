#include <Arduino.h>
#include "LittleFS.h"
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

typedef const char* cstr;

ESP8266WiFiMulti wifiMulti;

#include <devices/TestDevice.h>

DynamicJsonDocument doc(1024);
JsonObject root;
RequestHandler* sock;

void setup() {
	Serial.begin(9600);
	LittleFS.begin();
	Serial.println();
	delay(1000);

	File f = LittleFS.open("/settings.json", "r");
	if(!f) { Serial.println("config load failed! halting!"); delay(UINT_MAX); }

	deserializeJson(doc, f.readString());
	f.close();

	root = doc.as<JsonObject>();

	WiFi.hostname((cstr)root["hostname"]);

	JsonArray wifiData = root["wifi"];

	for(JsonObject station : wifiData)
		wifiMulti.addAP(station["ssid"], station["password"]);

	Serial.print("Connecting to Wifi...");
	while(wifiMulti.run() != WL_CONNECTED) Serial.print(".");

	Serial.println();
	Serial.printf("Connected to '%s'! IP Address: '%s'\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());

	Serial.printf("Connecting to Socket.IO Server at %s:%d ...\n", (cstr)root["server"]["host"], (short)root["server"]["port"]);

	JsonObject srv = root["server"];

	sock = new RequestHandler(srv["host"], srv["port"], new TestDevice(), srv["ssl"], srv["namespace"]);
	
	sock->onConnect([](JsonObject dataToSend){
		dataToSend["id"] = root["secretID"];
		Serial.println("Connected to Server!");
	});

	sock->onDisconnect([](){
		Serial.println("Server Disconnected!");
	});

	Serial.println("init done!");

}

void loop() {
	sock->loop();
}