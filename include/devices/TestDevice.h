#ifndef TESTDEVICEHDR
#define TESTDEVICEHDR

#include "../Device.h"

class TestDevice : public Device {

	Task* repeatingTask;

	void registerAllEvents();

	void light(JsonObjectConst data, JsonObject result);
	void testHandler(JsonObjectConst data, JsonObject result);
	void repeatingTest();
};

#endif