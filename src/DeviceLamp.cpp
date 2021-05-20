#include "DevicesDefinitions.h"

void BuildIn_LED_Switch(JsonObjectConst data, JsonObject result){
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

void DeviceLamp::registerAllEvents(RequestHandler* handler){
    pinMode(LED_BUILTIN, OUTPUT);
	handler->registerEvent("LightOn", BuildIn_LED_Switch);
}