#include "TestController.h"
#include "../SerialOutput.h"

TestController::TestController(): testHelpFunctions{&serial}, testEEPROMService {&serial} {

}

void TestController::initTests(int baudRate) {
	serial.initSerialLog(baudRate);
	testHelpFunctions.initTests();
	testEEPROMService.initTests();
	delay(1000);
    while(!Serial);
}

void TestController::runTests() {
	testHelpFunctions.runTests();
	testEEPROMService.runTests();
	delay(30000);
}
