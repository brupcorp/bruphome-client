#include <Arduino.h>
#include "ArduinoJson.h"
#include "LittleFS.h"
#include <ESP8266WiFi.h>

// TODO: unlink from library
#include <SocketIOclient.h>

#include "handlers.h"

DynamicJsonDocument settings(1024);
RequestHandler* sock;

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
	sock = new RequestHandler(settings["server"]["host"], settings["server"]["port"]);
	
	Serial.println("init done!");

}

void loop() {
	sock->loop();
}

void connectedHandler(JsonObjectConst, JsonObject result){
	result["id"] = settings["secretID"];
	Serial.println("connected");
}

void disconnectedHandler(JsonObjectConst,JsonObject){
	Serial.println("disconnected");
}

void testHandler(JsonObjectConst data, JsonObject result){
	Serial.print("got test! data: ");
	serializeJson(data, Serial);
	Serial.println();
	result.set(data);
}

void light(JsonObjectConst data, JsonObject result){
	pinMode(LED_BUILTIN, OUTPUT);
	if(data["status"] == "on"){
		digitalWrite(LED_BUILTIN, 0);
		Serial.println("LichtStatus: an");
		result["LEDStatus"] = "an";
	}
	else if(data["status"] == "off"){
		digitalWrite(LED_BUILTIN, 1);
		Serial.println("LichtStatus: aus");
		result["LEDStatus"] = "aus";
	}
	else{
		Serial.println("LichtStatus: unknown");
		if(digitalRead(LED_BUILTIN)==0){
			result["LEDStatus"] = "an";
		}
		else{
			result["LEDStatus"] = "aus";
		}
	}
}

void RequestHandler::registerAllEvents(){
	registerEvent("connected", connectedHandler);
	registerEvent("disconnected", disconnectedHandler);
	registerEvent("test", testHandler);
	registerEvent("LightOn", light);
}