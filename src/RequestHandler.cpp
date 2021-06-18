//#define DEBUGSOCK
#include "RequestHandler.h"

void RequestHandler::wsHandle(EventType type, const char* payload, unsigned length, void* additional){
	RequestHandler* h = (RequestHandler*)additional;
	h->req.clear();
	h->req.to<JsonObject>();
	switch(type){
		case EventType::Connect:{
			JsonArray final = h->req["final"].to<JsonArray>();
			final.add("handshake");
			h->connectionEstablished(final.createNestedObject());
			String s;
			serializeJson(final, s);
			h->sock.sendSocketIOEvent(s);
			return;
		}
		case EventType::Event:{
			deserializeJson(h->req, payload);
			h->handleEvent(h->req[0].as<String>(), h->req[1].as<JsonObject>());
			return;
		}
		case EventType::Disconnect:{
			h->disconnectedClient();
			return;
		}
		default: return;
	}
}

void RequestHandler::handleEvent(String eventName, JsonObject request){
	JsonArray out = request["final"].to<JsonArray>();
	out.add(request["requestID"]); // copy over requestID and free up
	request.remove("requestID");

	JsonObject response = out.createNestedObject();

	for(event& e : events){
		if(eventName == e.key){
			e.value->Invoke(request["data"], response);
			goto send;
		}
	}

	response["error"] = "invalid request!";

send:
	request.remove("data");
	if(eventName == "disconnected") return; // temp fix
	String s;
	serializeJson(request["final"], s);
	sock.sendSocketIOEvent(s);
}

void RequestHandler::setup(String host, short port, Device* device, bool useSSL, const char* ns){
	myDevice = device;
	sock.init(host, port, useSSL, ns == 0 ? "/" : ns);
	sock.additionalData = this;
	sock.onEvent(RequestHandler::wsHandle);
	device->linkHandler(this);
	device->registerAllEvents();
}

void RequestHandler::onConnect(event_handler_out handler){
	connectionEstablished = handler;
}

void RequestHandler::onDisconnect(callback handler){
	disconnectedClient = handler;
}

void RequestHandler::loop() { 
	sock.loop();
	for(Task* e : scheduler) e->tick();
}

void RequestHandler::registerEvent(String name, EventInvoker* handler) {
	events.push_back({name, handler});
};

void RequestHandler::registerRepeatingTask(Task* handler) {
	scheduler.push_back(handler);
};