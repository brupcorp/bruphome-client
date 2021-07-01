#ifndef NEOPIXELTESTHDR
#define NEOPIXELTESTHDR

#include "../Device.h"
#include "../Init.h"

class NeoPixelTest : public Init {
    public:
    int RBColorNum = 0;
    int colorInit = 0;

    void registerAllEvents();
    void testHandler(JsonObjectConst data, JsonObject result);

    void LEDOn(JsonObjectConst data, JsonObject result);
    void AllOn(JsonObjectConst data, JsonObject result);
    void AllOff(JsonObjectConst data, JsonObject result);
    
    void OnAllRBfade(JsonObjectConst data, JsonObject result);
    void AllRBfade();

    void OnAllRB(JsonObjectConst data, JsonObject result);
    void AllRainbow();

    static void reload();
    void SuspAll();
    
    Task* TaskAllRBfade;
    Task* TaskAllRainbow;
	

};

#endif