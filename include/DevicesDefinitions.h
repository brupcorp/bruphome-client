#ifndef DEVICESHDR
#define DEVICESHDR

#include "Device.h"
#include "RequestHandler.h"

MKDevice(TestDevice);
MKDevice(DeviceLamp);
MKSubDevice(RGB_Lamp, DeviceLamp);

#endif