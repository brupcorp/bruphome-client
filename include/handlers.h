#ifndef HANDLERSHDR
#define HANDLERSHDR

#include "WString.h"
#include "ArduinoJson.h"
#include <SocketIOclient.h>

#define str(v) (v.as<String>())

typedef void (*event_handler)(JsonObjectConst,JsonObject);

struct eventmap { String key; event_handler handler; };

class RequestHandler{
	private:
		std::vector<eventmap> events;
		SocketIOclient sock;
		static void wsHandle(socketIOmessageType_t type, byte* payload, size_t length, void* additional);
		void handleEvent(String event, JsonObject data);
		void registerEvent(String name, event_handler handler) { events.push_back({name, handler}); };
		void registerAllEvents();
	public:
		RequestHandler(String host, short port);
		void loop() { sock.loop(); };
};

#endif