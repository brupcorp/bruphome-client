#ifndef HANDLERSHDR
#define HANDLERSHDR

#include "WString.h"
#include "ArduinoJson.h"
#include <Arduino.h>
#include <SockIO.h>
#include "Device.h"
#include "EventInvoker.h"
#include "Task.h"

template <class T, class K> struct pair { T key; K value; };

typedef void (*event_handler_out)(JsonObject);
typedef void (*callback)();
typedef void (*invoker)(JsonObjectConst,JsonObject,void*);
typedef pair<String, EventInvoker*> event;

class RequestHandler{
	private:
		event_handler_out connectionEstablished = 0;
		callback disconnectedClient = 0;
		std::vector<event> events;
		std::vector<Task*> scheduler;
		SocketIO sock;
		Device* myDevice;
		StaticJsonDocument<2048> req;

		static void wsHandle(EventType type, const char* payload, unsigned length, void* additional);
		void handleEvent(String event, JsonObject data);
	public:
		// allow for static allocation and prepare object setup later
		void setup(String host, short port, Device* device, bool useSSL, const char* = "/");
		void onConnect(event_handler_out handler);
		void onDisconnect(callback handler);
		void loop();
		void registerEvent(String name, EventInvoker* handler);
		void registerRepeatingTask(Task* task);
};

#endif