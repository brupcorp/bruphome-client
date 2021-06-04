#ifndef DEVICEHDR
#define DEVICEHDR

class RequestHandler;

class Device{
	private:
		virtual void registerAllEvents(RequestHandler* handler) = 0;

	friend class RequestHandler;
};

#include "RequestHandler.h"
#define bindEvent(x) (new EventInvoker(&x, this))

#endif