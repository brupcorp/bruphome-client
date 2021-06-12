#ifndef NEOPIXELTESTHDR
#define NEOPIXELTESTHDR

#include "../Device.h"

class NeoPixelTest : public Device {

    void registerAllEvents();

    void testHandler(JsonObjectConst data, JsonObject result);

    void LEDOn(JsonObjectConst data, JsonObject result);

	

};

#endif