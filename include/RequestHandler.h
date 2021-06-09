#ifndef HANDLERSHDR
#define HANDLERSHDR

#include "WString.h"
#include "ArduinoJson.h"
#include <Arduino.h>
#include <SockIO.h>
#include "Device.h"
#include "EventInvoker.h"
#include "Task.h"

#define str(v) (v.as<String>())

typedef void (*event_handler_out)(JsonObject);
typedef void (*callback)();
typedef void (*invoker)(JsonObjectConst,JsonObject,void*);

struct event;

class RequestHandler{
	private:
		event_handler_out connectionEstablished = 0;
		callback disconnectedClient = 0;
		std::vector<event> events;
		std::vector<Task*> scheduler;
		SocketIO sock;
		Device* myDevice;

		static void wsHandle(EventType type, const char* payload, unsigned length, void* additional);
		void handleEvent(String event, JsonObject data);
	public:
		RequestHandler(String host, short port, Device* device, bool useSSL);
		void onConnect(event_handler_out handler);
		void onDisconnect(callback handler);
		void loop();
		void registerEvent(String name, EventInvoker* handler);
		void registerRepeatingTask(Task* task);
};

#endif