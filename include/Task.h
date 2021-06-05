#ifndef TASKHDR
#define TASKHDR

#include "EventInvoker.h"

class RequestHandler;

class Task{
	private:
		TaskInvoker* h;
		uint64 lastExecuted = 0;
		uint64 delayExec;
		bool suspended = false;

		void tick(){
			if(millis() - lastExecuted > delayExec && !suspended){
				lastExecuted = millis();
				h->Invoke();
			}
		}

	public:
		Task(TaskInvoker* handler, uint64 delay){
			h = handler;
			delayExec = delay;
		};

		uint64 getDelay() { return delayExec; };
		void setDelay(uint64 delay) { delayExec = delay; };
		bool getSuspended() { return suspended; };
		void setSuspended(bool suspend) { suspended = suspend; };

	friend class RequestHandler;

};

#endif
