#ifndef HANDLERSHDR
#define HANDLERSHDR

#include "WString.h"
#include "ArduinoJson.h"
#include <SocketIOclient.h>
#include "Device.h"
#include "EventInvoker.h"

#define str(v) (v.as<String>())

typedef void (*event_handler_out)(JsonObject);
typedef void (*callback)();
typedef void (*invoker)(JsonObjectConst,JsonObject,void*);

struct eventmap { String key; EventInvoker* handler; };

class RequestHandler{
	private:
		event_handler_out connectionEstablished = 0;
		callback disconnectedClient = 0;
		std::vector<eventmap> events;
		SocketIOclient sock;
		Device* myDevice;

		static void wsHandle(socketIOmessageType_t type, byte* payload, size_t length, void* additional);
		void handleEvent(String event, JsonObject data);
	public:
		RequestHandler(String host, short port, Device* device);
		void onConnect(event_handler_out handler);
		void onDisconnect(callback handler);
		void loop() { sock.loop(); };
		void registerEvent(String name, EventInvoker* handler) { events.push_back({name, handler}); };
};

#endif