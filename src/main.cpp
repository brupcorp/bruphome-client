#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
typedef const char* cstr;

ESP8266WiFiMulti wifiMulti;

#include <devices/TestDevice.h>

StaticJsonDocument<1024> doc;
JsonObject root;
RequestHandler sock;
byte settings[1024];

extern "C" uint32_t _FS_start;
#define FS_REL_ADDR ((uint32_t) (&_FS_start) - 0x40200000)

void setup() {
	Serial.begin(9600);
	Serial.println();
	delay(1000);

	ESP.flashRead(FS_REL_ADDR, settings, 1024);

	if(deserializeJson(doc, settings) != DeserializationError::Ok) {
		Serial.println("config load failed! halting!");
		delay(UINT_MAX);
	}

	root = doc.as<JsonObject>();
	
	WiFi.hostname((cstr)root["hostname"]);

	JsonArray wifiData = root["wifi"];

	for(JsonObject station : wifiData)
		wifiMulti.addAP(station["ssid"], station["password"]);

	Serial.print("Connecting to Wifi...");
	while(wifiMulti.run() != WL_CONNECTED) {
		delay(500); // doesn't flood every time but just to be sure
		Serial.print(".");
	}

	Serial.println();
	Serial.printf("Connected to '%s'! IP Address: '%s'\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());

	Serial.printf("Connecting to Socket.IO Server at %s:%d ...\n", (cstr)root["server"]["host"], (short)root["server"]["port"]);

	JsonObject srv = root["server"];

	sock.setup(srv["host"], srv["port"], new TestDevice(), srv["ssl"], srv["namespace"]);
	
	sock.onConnect([](JsonObject dataToSend){
		dataToSend["id"] = root["secretID"];
		Serial.println("Connected to Server!");
	});

	sock.onDisconnect([](){
		Serial.println("Server Disconnected!");
	});

	Serial.println("init done!");

}

void loop() { sock.loop(); }