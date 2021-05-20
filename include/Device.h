#ifndef DEVICEHDR
#define DEVICEHDR

class RequestHandler;

class Device{
	protected:
		virtual void registerAllEvents(RequestHandler* handler) = 0;

	friend class RequestHandler;
};

#define MKSubDevice(x, y) class x  : public y { protected: virtual void registerAllEvents(RequestHandler* handler);}
#define MKDevice(x) MKSubDevice(x, Device)

#endif