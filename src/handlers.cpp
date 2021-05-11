#include "handlers.h"

#define str(v) (v.as<String>())

void testHandler(JsonDocument& dat){
	Serial.print("called test! params: ");
	Serial.println(str(dat));
}

void connected(JsonDocument&){
	Serial.println("connected");

	StaticJsonDocument<256> test;
	test["isendData"] = true;
	test["test"]["penis"] = 123;

	wsSend("msg", test);
}

void disconnected(JsonDocument&){
	Serial.println("disconnected");
}