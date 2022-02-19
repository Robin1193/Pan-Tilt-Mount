#include "HelpFunctions.h"

double HelpFunctions::mapNumber(double x, double in_min, double in_max, double out_min, double out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

double HelpFunctions::radsToDeg(double rads) {
	return rads * 180 / PI;
}

double HelpFunctions::degToRads(double deg) {
	return deg * PI / 180;
}