#include <devices/TestDevice.h>

void TestDevice::testHandler(JsonObjectConst data, JsonObject result){
	Serial.print("got test! data: ");
	serializeJson(data, Serial);
	Serial.println();
	result.set(data);
}

void TestDevice::light(JsonObjectConst data, JsonObject result){
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

void TestDevice::registerAllEvents(RequestHandler* handler){
	handler->registerEvent("test", bindEvent(TestDevice::testHandler));
	handler->registerEvent("LightOn", bindEvent(TestDevice::light));
}