#include "DevicesDefinitions.h"

void testHandler(JsonObjectConst data, JsonObject result){
	Serial.print("got test! data: ");
	serializeJson(data, Serial);
	Serial.println();
	result.set(data);
}


void TestDevice::registerAllEvents(RequestHandler* handler){
	handler->registerEvent("test", testHandler);
}