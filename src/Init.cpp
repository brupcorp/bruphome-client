#include <Init.h>
#include <Adafruit_NeoPixel.h>
#include "devices/NeoPixelTest.h"

int Init::pin = 5;
int Init::numPixels = 20;
    
Init::Init(){

}


void Init::init(JsonObjectConst data, JsonObject result){
    Init::pin = data["Pin"];
    Init::numPixels = data["NumPixels"];

    NeoPixelTest::reload();

    result["Init Pin"] = Init::pin;
    result["Init NumPixels"] = Init::numPixels;

}


void Init::registerAllEvents(){
    handler->registerEvent("Init", bindEvent(Init::init));
}