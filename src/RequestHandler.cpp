#include "RequestHandler.h"

struct event { String key; EventInvoker* handler; };

void RequestHandler::wsHandle(socketIOmessageType_t type, byte* payload, size_t length, void* additional){
	RequestHandler* h = (RequestHandler*)additional;
	StaticJsonDocument<4096> doc;
	doc.to<JsonObject>();
	if(type == sIOtype_CONNECT){
		JsonArray final = doc["final"].to<JsonArray>();
		final.add("handshake");
		h->connectionEstablished(final.createNestedObject());
		String s;
		serializeJson(final, s);
		h->sock.sendEVENT(s);
	}else if(type == sIOtype_DISCONNECT){
		h->disconnectedClient();
		delay(15000);
		((WebSocketsClient*)&(h->sock))->disconnect(); // wtf is this line - cursed af
	}else if(type == sIOtype_EVENT){
		deserializeJson(doc, (char*)payload);
		h->handleEvent(str(doc[0]), doc[1].as<JsonObject>());
	}
}

void RequestHandler::handleEvent(String eventName, JsonObject request){
	JsonArray out = request["final"].to<JsonArray>();
	out.add(request["requestID"]); // copy over requestID and free up
	request.remove("requestID");

	JsonObject response = out.createNestedObject();

	for(event& e : events){
		if(eventName == e.key){
			e.handler->Invoke(request["data"], response);
			goto send;
		}
	}

	response["error"] = "invalid request!";

send:
	request.remove("data");
	if(eventName == "disconnected") return; // temp fix
	String s;
	serializeJson(request["final"], s);
	sock.sendEVENT(s);

}

RequestHandler::RequestHandler(String host, short port, Device* device, bool useSSL){
	myDevice = device;
	if(useSSL) sock.beginSSL(host, port);
	else sock.begin(host, port);
	sock.additional = this;
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