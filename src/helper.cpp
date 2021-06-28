#include <helper.h>


int helper::HexToInt(String data){
	const char *str = data.c_str();
	int r, g, b;
	sscanf(str, "%02x%02x%02x", &r, &g, &b);
	int ColorInt = r<<16 | g<<8 | b;
	return ColorInt;
}