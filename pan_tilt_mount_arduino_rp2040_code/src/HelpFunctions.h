#ifndef HELPFUNCTIONS_H
#define HELPFUNCTIONS_H

#if ARDUINO > 22
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


class HelpFunctions {
public:
    static double mapNumber(double x, double in_min, double in_max, double out_min, double out_max);
    static double degToRads(double deg);
    static double radsToDeg(double rads);
};

#endif
