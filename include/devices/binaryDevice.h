#ifndef BINARYDEVICEHDR
#define BINARYDEVICEHDR

#include "../Device.h"

class BinaryDevice : public Device {
	void registerAllEvents();
	int pin;
	bool pinStatusOn;

	void switch0(JsonObjectConst data, JsonObject result);
	void testHandler(JsonObjectConst data, JsonObject result);
};

#endif