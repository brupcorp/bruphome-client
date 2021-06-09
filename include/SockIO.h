#ifndef SOCKIOHDR
#define SOCKIOHDR

#include <Arduino.h>
#include <base64.h>
#include <ESP8266WiFi.h>
#include <Hash.h>

#include "wshelper.h"

#ifdef DEBUGSOCK
#define Debug(...) Serial.printf(__VA_ARGS__)
#else
#define Debug(...) \
	{}
#endif

enum class EventType {
	Connect,
	Disconnect,
	Binary,
	Event,
};

typedef void (*eventHandler)(EventType, const char*, unsigned, void*);

class SocketIO {

  private:
	eventHandler handler = 0;
	String ns;
	String host;
	short port;

	String path = "/socket.io/?EIO=4&transport=websocket";

	String secKey;
	Client* c;

	bool lastConnectedState = false;
	bool connected() { return c->connected(); }

	int state = 0;
	bool headerOk = false;

	unsigned lastConnectionAttempt = INT_MAX;
	unsigned waitTime = 30; // wait 30 seconds between reconnects to not spam the server

	char buffer[14] = {0};
	char* curBuffPtr = 0;
	char* payloadBuffer = 0;
	unsigned wsReadState = 0;

	WSMessageHeader_t wsHeader = {0};

	bool validateKey(String compare) {
		uint8_t sha1HashBin[20] = {0};
		sha1(secKey + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11", &sha1HashBin[0]);
		String key = base64::encode(sha1HashBin, 20, false);
		key.trim();
		Debug("expected key: '%s'\n", key.c_str());
		return key == compare;
	}

	bool sendFrame(WSopcode_t opcode, const char* payload, unsigned length){
		if(!connected() || state != 1) return false;

		unsigned maskKey = random(ULONG_MAX);
		char* mask = (char*)&maskKey;
		char frameHeader[14] = { 0 };
		char* cur;
		char* hdrBegin;
		if(length < 126) cur = &frameHeader[7];
		else if (length < 0xffff) cur = &frameHeader[5];
		else cur = &frameHeader[0];

		hdrBegin = cur;
		unsigned hdrLen = &frameHeader[13] - hdrBegin;

		*cur = 1 << 7;
		*cur++ |= opcode;

		*cur = 1 << 7;

		if(length < 126) *cur++ |= length;
		else if (length < 0xffff) {
			*cur++ |= 126;
			*((short*)cur++) = length;
			cur++;
		}else{
			*cur++ |= 127;
			*((unsigned*)cur) = 0;
			cur += 4;
			*((unsigned*)cur) = length;
			cur += 4;
		}

		char* m = mask; // casting to int and writing causes unaligned write crash
		*cur++ = *m++;
		*cur++ = *m++;
		*cur++ = *m++;
		*cur++ = *m++;

		char* dataCopy = new char[length];
		memcpy(dataCopy, payload, length);

		for(size_t x = 0; x < length; x++)
            dataCopy[x] = (dataCopy[x] ^ mask[x % 4]);

		if(c->write((uint8_t*)hdrBegin, hdrLen) != hdrLen) return false;
		if(c->write((uint8_t*)dataCopy, length) != length) return false;

		delete[] dataCopy;

		Debug("packet sent successfully\n");

		return true;
	}

	bool sendText(const String& text){
		Debug("sending text '%s'\n", text.c_str());
		return sendFrame(WSop_text, text.c_str(), text.length());
	}

	bool sendMessage(const String& text){
		return sendText(String("4") + text);
	}

	void handleSocketIO(String payload){
		Debug("socket io message received: %s\n", payload.c_str());
		if(handler) {
			switch(payload[0] - '0'){
				case 0:
					handler(EventType::Connect, 0, 0, additionalData);
					return;
				case 2:{
					unsigned begin = payload.indexOf('['); // crude parser
					unsigned len = payload.length() - begin;
					handler(EventType::Event, &payload[begin], len, additionalData);
					return;
				}
			}
		}
	}

	void handleConnectSocketIO(){
		sendMessage(String("0") + ns + ","); // join namespace
	}

	void handleWebsocketMessage() {
		switch(wsHeader.opCode) {
		case WSop_text: {
			Debug("received text: %s\n", payloadBuffer);
			switch (payloadBuffer[0] - '0')
			{
			case 0:
				handleConnectSocketIO();
				return;
			case 2:
				sendText("3"); // engine.io ping signal
				return;
			case 4:
				handleSocketIO(&payloadBuffer[1]);
				return;
			}
		}
		case WSop_binary: {
			Debug("received bin! len: %d\n", wsHeader.payloadLen);
			// handle binary
			return;
		}
		case WSop_continuation: {
			Debug("received continuation\n");
			return;
		}
		case WSop_ping: {
			sendFrame(WSop_pong, payloadBuffer, wsHeader.payloadLen);
			Debug("received ping\n");
			return;
		}

		case WSop_pong: {
			Debug("received pong\n");
			return;
		}

		case WSop_close: {
			Debug("received close request\n");
			return;
		}
		}
	}

  public:
  	void* additionalData = 0;

	void init(String host, short port, bool ssl, String ns = "/") {
		this->host = host;
		this->port = port;
		this->ns = ns;

		if(ssl) c = new WiFiClientSecure();
		else c = new WiFiClient();

		uint8_t randomKey[16] = {0};

		for(uint8_t i = 0; i < sizeof(randomKey); i++) randomKey[i] = random(0xFF);

		secKey = base64::encode(&randomKey[0], 16, false);
		secKey.trim();
	}

	void onEvent(eventHandler handler) { this->handler = handler; }

	bool sendSocketIOEvent(const String& json){
		Debug("sending socketio message: '%s'\n", json.c_str());
		return sendMessage(String("2") + ns + "," + json);
	}

	void loop() {
		if(connected()) {
			lastConnectedState = true;
			if(c->available() > 0) {
				switch(state) {
				case 0: {
					String hdr = c->readStringUntil('\n');
					hdr.trim(); // remove \r

					if(hdr.startsWith("HTTP/1.1")) {
						headerOk = hdr.endsWith("101 Switching Protocols");
						Debug("Is Switching Protocols: %d\n", headerOk);
						break;
					}

					if(hdr == "") {
						if(headerOk) {
							state = 1; // switch to connected websocket state
							Debug("websocket is now in connected state!\n");
						} else {
							c->stop();
							state = 0;
							Debug("rejecting connection! something is not right\n");
						}

						break;
					}

					String headerName = hdr.substring(0, hdr.indexOf(':'));
					String headerValue = hdr.substring(hdr.indexOf(':') + 1);
					if(headerValue[0] == ' ') headerValue.remove(0, 1); // remove space in the beginning  (RFC2616)

					Debug("Header: '%s' Value: '%s'\n", headerName.c_str(), headerValue.c_str());

					if(headerName == "Sec-WebSocket-Accept") headerOk = validateKey(headerValue);

					break;
				}

				case 1: {
					switch(wsReadState) {
					case 0: { // initial header parse state
						if(c->available() < 2) return;
						c->readBytes(buffer, 2);

						curBuffPtr = buffer;
						wsHeader.fin = ((*curBuffPtr >> 7) & 0x01);
						wsHeader.rsv1 = ((*curBuffPtr >> 6) & 0x01);
						wsHeader.rsv2 = ((*curBuffPtr >> 5) & 0x01);
						wsHeader.rsv3 = ((*curBuffPtr >> 4) & 0x01);
						wsHeader.opCode = (WSopcode_t)(*curBuffPtr & 0x0F);
						curBuffPtr++;

						wsHeader.mask = ((*curBuffPtr >> 7) & 0x01);
						wsHeader.payloadLen = (WSopcode_t)(*curBuffPtr & 0x7F);
						curBuffPtr++;

						if(wsHeader.payloadLen == 126) wsReadState = 1;
						else if(wsHeader.payloadLen == 127)
							wsReadState = 2;
						else if(wsHeader.mask)
							wsReadState = 3;
						else
							wsReadState = 4;

						break;
					}
					case 1: {						  // read extended length 1
						if(c->available() < 2) return; // wait for two additional bytes
						c->readBytes(curBuffPtr, 2);
						wsHeader.payloadLen = *(short*)curBuffPtr;
						curBuffPtr += 2;

						if(wsHeader.mask) wsReadState = 3;
						else
							wsReadState = 4;

						break;
					}
					case 2: {						  // read extended length 2
						if(c->available() < 8) return; // wait for 8 additional bytes
						c->readBytes(curBuffPtr, 8);

						if(*((unsigned*)buffer[2]) != 0) wsHeader.payloadLen = 0xFFFFFFFF; // really too big!
						else
							wsHeader.payloadLen = *((unsigned*)buffer[2]);

						curBuffPtr += 8;

						if(wsHeader.mask) wsReadState = 3;
						else
							wsReadState = 4;

						break;
					}
					case 3: {						  // read mask key
						if(c->available() < 4) return; // wait for 4 mask key bytes
						c->readBytes(curBuffPtr, 4);
						wsHeader.maskKey = curBuffPtr;
						wsReadState = 4;
						break;
					}
					case 4: { // read the payload
						if(c->available() < (int)wsHeader.payloadLen) return;
						if(payloadBuffer) delete[] payloadBuffer;
						payloadBuffer = new char[wsHeader.payloadLen + 1];
						payloadBuffer[wsHeader.payloadLen] = 0; // nullterminate for safety
						c->readBytes(payloadBuffer, wsHeader.payloadLen);

						if(wsHeader.mask) {
							// decode XOR
							for(size_t i = 0; i < wsHeader.payloadLen; i++)
								payloadBuffer[i] = (payloadBuffer[i] ^ wsHeader.maskKey[i % 4]);
						}
						wsReadState = 0;

						// TODO: clear wsBuffer

						handleWebsocketMessage();
						break;
					}
					}

					break;
				}

				default: break;
				}
			}
		} else {
			if(lastConnectedState && handler) handler(EventType::Disconnect, 0, 0, additionalData);
			lastConnectedState = false;
			if(millis() - lastConnectionAttempt < waitTime * 1000) return;
			String request = "GET " + path + " HTTP/1.1\r\n";
			request += "Host: " + host + ":" + port + "\r\n";
			request += "Connection: Upgrade\r\n";
			request += "Upgrade: websocket\r\n";
			request += "Sec-WebSocket-Version: 13\r\n";
			request += "Sec-WebSocket-Key: " + secKey + "\r\n\r\n";

			Debug("Sending Request:\n%s", request.c_str());

			c->connect(host.c_str(), port);
			c->write((uint8_t*)request.c_str(), request.length());
			state = 0; // initial state
			lastConnectionAttempt = millis();
		}
	}
};

#endif