#include <devices/NeoPixelTest.h>
#include <Adafruit_NeoPixel.h>
#include <helper.h>

void init(JsonObjectConst data, JsonObject result){
	

}
#define PIN D1
#define NUMPIXELS 20 

Adafruit_NeoPixel pixels(NUMPIXELS, D1, NEO_GRB + NEO_KHZ800);


void NeoPixelTest::testHandler(JsonObjectConst data, JsonObject result){
	SuspAll();

	Serial.print("got test! data: ");
	serializeJson(data, Serial);
	Serial.println();
	result.set(data);
}

void NeoPixelTest::LEDOn(JsonObjectConst data, JsonObject result){
	SuspAll();

	pixels.begin();
	if(data.containsKey("RGBColor") && data.containsKey("LEDNum")){
		pixels.setPixelColor(data["LEDNum"], helper::HexToInt(data["RGBColor"]));

		pixels.show();
		result["LEDStatus"] = "an";
	}
}

void NeoPixelTest::AllOn(JsonObjectConst data, JsonObject result){
	SuspAll();

	pixels.begin();
	if(data.containsKey("RGBColor")){
		for(int i = 0; i<NUMPIXELS; i++){
			pixels.setPixelColor(i, helper::HexToInt(data["RGBColor"]));
		};
		pixels.show();
		result["LEDStatus"] = "an";
	}
	else{
		result["LEDStatus"] = "aus-error";
	}
}

void NeoPixelTest::AllOff(JsonObjectConst data, JsonObject result){
	SuspAll();

	pixels.begin();
	pixels.clear();
	pixels.show();


	result["LEDStatus"] = "aus";

}

void NeoPixelTest::OnAllRB(JsonObjectConst data, JsonObject result){
	SuspAll();
	TaskAllRainbow->setSuspended(false);
	result["Suspended:"] = "aus";
}

void NeoPixelTest::AllRainbow(){
	int RBColors[6] = {0xfcba03, 0xa80816, 0xe100ff, 0x0cad0c, 0x070b87, 0x07f58a};
	pixels.begin();
	for(int i = 0; i<NUMPIXELS; i++){
		pixels.setPixelColor(i, RBColors[RBColorNum]);
	};
	pixels.show();
	if(RBColorNum < 5){
		RBColorNum = RBColorNum + 1;
	} else {
		RBColorNum = 0;
	}
	
	

}

void NeoPixelTest::SuspAll(){
	TaskAllRainbow->setSuspended(true);

}

void NeoPixelTest::registerAllEvents(){
	TaskAllRainbow = new Task(bindTask(NeoPixelTest::AllRainbow), 1000);
	SuspAll();

    handler->registerEvent("test", bindEvent(NeoPixelTest::testHandler));
    handler->registerEvent("NeoPOn", bindEvent(NeoPixelTest::LEDOn));
	handler->registerEvent("NeoAllOn", bindEvent(NeoPixelTest::AllOn));
	handler->registerEvent("NeoAllOff", bindEvent(NeoPixelTest::AllOff));

	handler->registerEvent("OnAllRB", bindEvent(NeoPixelTest::OnAllRB));
	handler->registerRepeatingTask(TaskAllRainbow);	

}