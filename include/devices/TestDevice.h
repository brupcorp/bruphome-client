#ifndef TESTDEVICEHDR
#define TESTDEVICEHDR

#include "../Device.h"

class TestDevice : public Device {

	Task* repeatingTask;
	Task* repeatingTask2;

	void registerAllEvents();

	void light(JsonObjectConst data, JsonObject result);
	void testHandler(JsonObjectConst data, JsonObject result);
	void repeatingTest();
	void repeatingTestTask2();
};

#endif