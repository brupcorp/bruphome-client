#include <Arduino.h>
#include "ArduinoJson.h"
#include "LittleFS.h"
#include <ESP8266WiFi.h>

// TODO: unlink from library
#include <SocketIOclient.h>

#define MAINHERE
#include "handlers.h"

#define str(v) (v.as<String>())

DynamicJsonDocument settings(1024);
SocketIOclient sock;

void messageHandler(String event, JsonDocument& data);

void wsSend(String event, JsonDocument& data){
	DynamicJsonDocument doc(2048);
	doc.add(event);
	doc.add(data.as<JsonObject>());
	String s;
	serializeJson(doc, s);
	sock.sendEVENT(s);
}

void wsEvent(socketIOmessageType_t type, byte* payload, size_t length){
	DynamicJsonDocument doc(2048);
	if(type == sIOtype_CONNECT)
		messageHandler("connected", doc);
	else if(type == sIOtype_DISCONNECT)
		messageHandler("disconnected", doc);
	else if(type == sIOtype_EVENT){
		deserializeJson(doc, (char*)payload);
		DynamicJsonDocument d = doc[1];
		messageHandler(str(doc[0]), d);
	}
}

void setup() {
	Serial.begin(9600);
	LittleFS.begin();
	Serial.println();
	delay(1000);

	File f = LittleFS.open("/settings.json", "r");
	if(!f) Serial.println("file open failed");

	deserializeJson(settings, f.readString());
	f.close();

	JsonObject wifi = settings["wifi"];

	WiFi.hostname(str(wifi["hostname"]));
	WiFi.begin(str(wifi["ssid"]), str(wifi["password"]));

	Serial.print("Connecting");
	while(WiFi.status() != WL_CONNECTED) {
		delay(1000);
		Serial.print(".");
	}
	Serial.println();

	Serial.print("Connected! IP address: ");
	Serial.println(WiFi.localIP());

	Serial.print("initiating socket.io connection at ");
	Serial.print(str(settings["server"]["host"]));
	Serial.print(":");
	Serial.println(settings["server"]["port"].as<short>());
	sock.begin(str(settings["server"]["host"]), settings["server"]["port"].as<short>());
	sock.onEvent(wsEvent);

	Serial.println("init done!");

}

void loop() {
	sock.loop();
}

void messageHandler(String event, JsonDocument& data){
	for(unsigned idx = 0; idx < (sizeof(events)/sizeof(eventmap)); idx++){
		if(events[idx].key == event){
			events[idx].handler(data);
			break;
		}
			
	}
}