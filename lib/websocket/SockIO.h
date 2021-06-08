#ifndef SOCKIOHDR
#define SOCKIOHDR

#include <Arduino.h>
#include <base64.h>
#include <ESP8266WiFi.h>
#include <Hash.h>

#ifdef DEBUGSOCK
#define Debug(...) Serial.printf(__VA_ARGS__)
#else
#define Debug(...)
#endif

enum class EventType {
	Connect,
	Disconnect,
	Binary,
	Event,
	Ping,
};

typedef void (*eventHandler)(EventType, char*);

class SocketIO {

  private:
	eventHandler handler;
	String ns;
	String host;
	short port;
	bool ssl;

	String path = "/socket.io/?EIO=4&transport=websocket";

	String secKey;
	WiFiClient c;

	bool connected() { return c.connected(); }

	int state = 0;

	bool validateKey(String compare) {
		uint8_t sha1HashBin[20] = { 0 };
		sha1(secKey + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11", &sha1HashBin[0]);
		String key = base64::encode(sha1HashBin, 20, false);
		key.trim();
		Debug("expected key: '%s'\n", key.c_str());
		return key==compare;
	}

  public:
	void init(String host, short port, bool ssl, String ns) {
		this->host = host;
		this->port = port;
		this->ns = ns;
		this->ssl = ssl;

		uint8_t randomKey[16] = {0};

		for(uint8_t i = 0; i < sizeof(randomKey); i++) randomKey[i] = random(0xFF);

		secKey = base64::encode(&randomKey[0], 16, false);
		secKey.trim();
	}

	void onEvent(eventHandler handler) { handler = handler; }

	void loop() {
		if(connected()) {
			if(c.available() > 0){
				switch(state) {
				case 0:{
					String hdr = c.readStringUntil('\n');
					hdr.trim(); // remove \r

					String headerName  = hdr.substring(0, hdr.indexOf(':'));
					String headerValue = hdr.substring(hdr.indexOf(':') + 1);
					if(headerValue[0] == ' ') headerValue.remove(0, 1); // remove space in the beginning  (RFC2616)

					Debug("Header: '%s' Value: '%s'\n", headerName.c_str(), headerValue.c_str());

					if(headerName == "Sec-WebSocket-Accept")
						Debug("key is valid: %d\n", validateKey(headerValue));

					if(hdr == "") state = 1; // switch to connected websocket state
					break;

				}
				default: break;
				}
			}
		} else {
			String request = "GET " + path + " HTTP/1.1\r\n";
			request += "Host: " + host + ":" + port + "\r\n";
			request += "Connection: Upgrade\r\n";
			request += "Upgrade: websocket\r\n";
			request += "Sec-WebSocket-Version: 13\r\n";
			request += "Sec-WebSocket-Key: " + secKey + "\r\n\r\n";

			Debug("Sending Request:\n%s", request.c_str());

			c.connect(host, port);
			c.write(request.c_str());
			state = 0; // initial state
		}
	}
};

#endif