#ifndef INITHDR
#define INITHDR
#include <Device.h>

class Init : public Device {

    public:

    Init();
    void registerAllEvents();
    void init(JsonObjectConst data, JsonObject result);

    static int pin;
    static int numPixels;


};


#endif