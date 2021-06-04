#ifndef TESTDEVICEHDR
#define TESTDEVICEHDR

#include "../Device.h"

class TestDevice : public Device {
	void registerAllEvents(RequestHandler* handler);

	void light(JsonObjectConst data, JsonObject result);
	void testHandler(JsonObjectConst data, JsonObject result);
};

#endif