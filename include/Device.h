#ifndef DEVICEHDR
#define DEVICEHDR

class RequestHandler;

class Device{
	private:
		void linkHandler(RequestHandler* bind) { handler = bind; };
	protected:
		RequestHandler* handler;
		virtual void registerAllEvents() = 0;
	friend class RequestHandler;
};

#include "RequestHandler.h"
#define bindEvent(x) (new EventInvoker(&x, this))
#define bindTask(x) (new TaskInvoker(&x, this))

#endif