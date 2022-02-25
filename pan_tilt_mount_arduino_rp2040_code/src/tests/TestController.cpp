#include "TestController.h"
#include "../SerialOutput.h"

TestController::TestController(): testHelpFunctions{&serial} {

}

void TestController::initTests(int baudRate) {
	serial.initSerialLog(baudRate);
	testHelpFunctions.initTests();
	delay(1000);
    while(!Serial);
}

void TestController::runTests() {
	testHelpFunctions.runTests();
	delay(30000);
}
