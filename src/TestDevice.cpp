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

void TestDevice::repeatingTest(){
	uint64 curDelay = repeatingTask->getDelay();
	Serial.printf("task! currentDelay: %llu\n", curDelay);
	repeatingTask->setDelay(curDelay + 500);
}

void TestDevice::registerAllEvents(){
	handler->registerEvent("test", bindEvent(TestDevice::testHandler));
	handler->registerEvent("LightOn", bindEvent(TestDevice::light));

	repeatingTask = new Task(bindTask(TestDevice::repeatingTest), 1000);

	handler->registerRepeatingTask(repeatingTask);

}