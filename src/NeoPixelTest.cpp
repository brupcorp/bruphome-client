#include <devices/NeoPixelTest.h>
#include <Adafruit_NeoPixel.h>
#include <helper.h>
#include <colorUtil.h>
#include <Init.h>

//TODO: On clear every LED possible clearen not just the numPixels

Adafruit_NeoPixel pixels(Init::numPixels, Init::pin, NEO_GRB + NEO_KHZ800);

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
		for(int i = 0; i<Init::numPixels; i++){
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
	for(int i = 0; i<Init::numPixels; i++){
		pixels.setPixelColor(i, RBColors[RBColorNum]);
	};
	pixels.show();
	if(RBColorNum < 5){
		RBColorNum = RBColorNum + 1;
	} else {
		RBColorNum = 0;
	}
}

void NeoPixelTest::OnAllRBfade(JsonObjectConst data, JsonObject result){
	SuspAll();
	TaskAllRBfade->setSuspended(false);
	result["Suspended:"] = "aus";
}

void NeoPixelTest::AllRBfade(){

    hsv color;
    color.h = colorInit;
    color.s = 1;
    color.v = 0.8;

    pixels.begin();
    for(int i = 0; i<Init::numPixels; i++){
        rgb converted = hsv2rgb(color);
        pixels.setPixelColor(i, converted.r * 255, converted.g * 255, converted.b * 255);
        color.h = ((int)color.h + 7) % 360; 
    };
    
    pixels.show();
    colorInit = (colorInit + 1) % 360;
    
}

void NeoPixelTest::SuspAll(){
	TaskAllRainbow->setSuspended(true);
	TaskAllRBfade->setSuspended(true);

}

void NeoPixelTest::reload(){
	pixels.setPin(Init::pin);
	pixels.updateLength(Init::numPixels);
}

void NeoPixelTest::registerAllEvents(){
	Init::registerAllEvents();
	TaskAllRainbow = new Task(bindTask(NeoPixelTest::AllRainbow), 1000);
	TaskAllRBfade = new Task(bindTask(NeoPixelTest::AllRBfade), 10);
	SuspAll();

    handler->registerEvent("test", bindEvent(NeoPixelTest::testHandler));
    handler->registerEvent("NeoPOn", bindEvent(NeoPixelTest::LEDOn));
	handler->registerEvent("NeoAllOn", bindEvent(NeoPixelTest::AllOn));
	handler->registerEvent("NeoAllOff", bindEvent(NeoPixelTest::AllOff));

	handler->registerEvent("OnAllRB", bindEvent(NeoPixelTest::OnAllRB));
	handler->registerEvent("OnAllRBFade", bindEvent(NeoPixelTest::OnAllRBfade));
	handler->registerRepeatingTask(TaskAllRainbow);	
	handler->registerRepeatingTask(TaskAllRBfade);

}