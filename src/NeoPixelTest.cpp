#include <devices/NeoPixelTest.h>
#include <Adafruit_NeoPixel.h>

#define PIN D1
#define NUMPIXELS 20 

Adafruit_NeoPixel pixels(NUMPIXELS, D1, NEO_GRB + NEO_KHZ800);

void NeoPixelTest::testHandler(JsonObjectConst data, JsonObject result){
	Serial.print("got test! data: ");
	serializeJson(data, Serial);
	Serial.println();
	result.set(data);
}

void NeoPixelTest::LEDOn(JsonObjectConst data, JsonObject result){

	pixels.begin();
	if(data.containsKey("RGBColor") && data.containsKey("LEDNum")){
		pixels.setPixelColor(data["LEDNum"], data["RGBColor"]);

		pixels.show();
		result["LEDStatus"] = "an";
	}
}

void NeoPixelTest::AllOn(JsonObjectConst data, JsonObject result){

	pixels.begin();
	if(data.containsKey("RGBColor")){
		for(int i = 0; i<NUMPIXELS; i++){
		pixels.setPixelColor(i, data["RGBColor"]);
		};
		pixels.show();
		result["LEDStatus"] = "an";
	}
	else{
		result["LEDStatus"] = "aus-error";
	}
}

void NeoPixelTest::AllOff(JsonObjectConst data, JsonObject result){

	pixels.begin();
	pixels.clear();
	pixels.show();


	result["LEDStatus"] = "aus";

}

void NeoPixelTest::AllRainbow(){
	uint64 curDelay = TaskAllRainbow->getDelay();
	


	TaskAllRainbow->setDelay(curDelay + 500);
}

void NeoPixelTest::registerAllEvents(){

    handler->registerEvent("test", bindEvent(NeoPixelTest::testHandler));
    handler->registerEvent("NeoPOn", bindEvent(NeoPixelTest::LEDOn));
	handler->registerEvent("NeoAllOn", bindEvent(NeoPixelTest::AllOn));
	handler->registerEvent("NeoAllOff", bindEvent(NeoPixelTest::AllOff));

	TaskAllRainbow = new Task(bindTask(NeoPixelTest::AllRainbow), 1000);

}