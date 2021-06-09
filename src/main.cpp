#include <Arduino.h>
#include "LittleFS.h"
#include <ESP8266WiFi.h>

#include <devices/TestDevice.h>

DynamicJsonDocument settings(1024);
RequestHandler* sock;

void setup() {
	Serial.begin(9600);
	LittleFS.begin();
	Serial.println();
	delay(1000);

	File f = LittleFS.open("/settings.json", "r");
	if(!f) { Serial.println("config load failed! halting!"); delay(UINT_MAX); }

	deserializeJson(settings, f.readString());
	f.close();

	JsonObject wifi = settings["wifi"];

	WiFi.hostname(str(wifi["hostname"]));
	WiFi.begin(str(wifi["ssid"]), str(wifi["password"]));

	Serial.print("Connecting");
	while(WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	Serial.println();
	Serial.print("Connected! IP address: ");
	Serial.println(WiFi.localIP());

	Serial.print("initiating socket.io connection at ");
	Serial.print(str(settings["server"]["host"]));
	Serial.print(":");
	Serial.println(settings["server"]["port"].as<short>());

	sock = new RequestHandler(settings["server"]["host"], settings["server"]["port"], new TestDevice(), settings["server"]["ssl"]);
	
	sock->onConnect([](JsonObject dataToSend){
		dataToSend["id"] = settings["secretID"];
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