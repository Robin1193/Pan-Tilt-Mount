#ifndef TESTHELPFUNCTIONS_H
#define TESTHELPFUNCTIONS_H

#include "../../SerialOutput.h"

class TestHelpFunctions {
public:
    TestHelpFunctions(SerialOutput* serialOutput);
    void initTests();
    void runTests();
private:
    SerialOutput* serial = NULL;
    void array_to_string(byte array[], unsigned int len, char buffer[]);
    template <typename T>
    void checkByteConvertion(T item);
    void testInt(int value);
    void testDouble(double value);
    void testBool(bool value);
    void testByte(byte value);
    void testFloat(float value);
    void testLong(long value);
};

#endif
