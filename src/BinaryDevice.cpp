#include <devices/BinaryDevice.h>

void BinaryDevice::testHandler(JsonObjectConst data, JsonObject result){
	Serial.print("got test! data: ");
	serializeJson(data, Serial);
	Serial.println();
	result.set(data);
}

void BinaryDevice::switch0(JsonObjectConst data, JsonObject result){
	pin = data["pinName"];
	pinMode(pin, OUTPUT);
	pinStatusOn = digitalRead(pin);
	digitalWrite(pin, !pinStatusOn);
}


void BinaryDevice::registerAllEvents(){
	handler->registerEvent("test", bindEvent(BinaryDevice::testHandler));
	handler->registerEvent("switch", bindEvent(BinaryDevice::switch0));
}