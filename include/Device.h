#ifndef DEVICEHDR
#define DEVICEHDR

class RequestHandler;

class Device{
	private:
		virtual void registerAllEvents(RequestHandler* handler) = 0;

	friend class RequestHandler;
};

#define MKDevice(x) class x  : public Device { void registerAllEvents(RequestHandler* handler);}
#endif