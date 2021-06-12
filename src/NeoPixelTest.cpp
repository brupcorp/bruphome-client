#include <devices/NeoPixelTest.h>
#include <Adafruit_NeoPixel.h>

#define PIN D1
#define NUMPIXELS 80

Adafruit_NeoPixel pixels(NUMPIXELS, D1, NEO_GRB + NEO_KHZ800);

void NeoPixelTest::testHandler(JsonObjectConst data, JsonObject result){
	Serial.print("got test! data: ");
	serializeJson(data, Serial);
	Serial.println();
	result.set(data);
}

void NeoPixelTest::LEDOn(JsonObjectConst data, JsonObject result){

	pixels.begin();
	pixels.setPixelColor(1, pixels.Color(0, 150, 0));
	pixels.show();

	result["LEDStatus"] = "an";

}

void NeoPixelTest::registerAllEvents(){

    handler->registerEvent("test", bindEvent(NeoPixelTest::testHandler));
    handler->registerEvent("NeoPOn", bindEvent(NeoPixelTest::LEDOn));

}