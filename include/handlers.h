#ifndef HANDLERSHDR
#define HANDLERSHDR

#include "WString.h"
#include "ArduinoJson.h"

typedef void (*event_handler)(JsonDocument&);

struct eventmap { String key; event_handler handler; };

// handlers forward declaration
void testHandler(JsonDocument&);
void connected(JsonDocument&);
void disconnected(JsonDocument&);

#ifdef MAINHERE

eventmap events[] = {
	{ "connected", connected },
	{ "disconnected", disconnected },
	{ "test", testHandler }
};

#endif

void wsSend(String event, JsonDocument& data);

#endif