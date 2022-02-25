#ifndef TESTCONTROLLER_H
#define TESTCONTROLLER_H

#if ARDUINO > 22
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "../SerialOutput.h"
#include "testfunctions/TestHelpFunctions.h"

class TestController {
public:
    TestController();
    void initTests(int baudRate);
    void runTests();
private:
    SerialOutput serial;
    TestHelpFunctions testHelpFunctions;
};

#endif
