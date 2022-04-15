#ifndef TESTEEPROMSERVICE_H
#define TESTEEPROMSERVICE_H

#include "../../SerialOutput.h"
#include "../../EEPROMService.h"

class TestEEPROMService {
public:
    TestEEPROMService(SerialOutput* serialOutput, TwoWire *wire = &Wire);
    void initTests();
    void runTests();
private:
void checkClear();
    template <typename T>
    void checkGetPutItem(T item);
    void testGetPutInt(int value);
    void testGetPutDouble(double value);
    void testGetPutBool(bool value);
    void testGetPutByte(byte value);
    void testGetPutFloat(float value);
    void testGetPutLong(long value);

    void testRead(byte testValue);

    SerialOutput* serial = NULL;
    EEPROMService eepromService;

    const int deviceSize = I2C_DEVICESIZE_24LC512;
};

#endif
