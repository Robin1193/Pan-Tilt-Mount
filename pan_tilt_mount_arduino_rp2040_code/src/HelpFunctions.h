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

    template <typename T>
    static void itemToBytes(T item, byte* bytes) {
        union {
        T t_variable;
        byte temp_array[sizeof(T)];
        } u;
        // Overite bytes of union with float variable
        u.t_variable = item;
        // Assign bytes to input array
        memcpy(bytes, u.temp_array, sizeof(T));
    }

   template <typename T>
    static void itemFromBytes(T* item, byte* bytes) {
        memcpy(item, bytes, sizeof(T));
    }

};

#endif
