#ifndef EVTINVOKERHDR
#define EVTINVOKERHDR

#include "ArduinoJson.h"

// fuck c++ and templates

class EventInvoker{
	private:
		void* helperData;
		void (*AliasInvoker)(JsonObjectConst,JsonObject,void*);

	public:

		void Invoke(JsonObjectConst in, JsonObject out) { AliasInvoker(in, out, helperData); };

		template<class T, class fptr>EventInvoker(fptr function, T* cls){
			struct helper { fptr f; T* d; };
			helperData = (void*)new helper({function, cls});
			AliasInvoker = [](JsonObjectConst i,JsonObject o,void* d){
				helper* h = (helper*)d;
				((h->d)->*(h->f))(i,o);
			};
		};

};

// fuck c++ and templates
class TaskInvoker{
	private:
		void* helperData;
		void (*AliasInvoker)(void*);

	public:

		void Invoke() { AliasInvoker(helperData); };

		template<class T, class fptr>
		TaskInvoker(fptr function, T* cls){
			struct helper { fptr f; T* d; };
			helperData = (void*)new helper({function, cls});
			AliasInvoker = [](void* d){
				helper* h = (helper*)d;
				((h->d)->*(h->f))();
			};
		};

};

#endif
