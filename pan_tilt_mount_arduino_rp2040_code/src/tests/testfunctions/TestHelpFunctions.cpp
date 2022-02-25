#include "TestHelpFunctions.h"
#include "../../SerialOutput.h"
#include "../../HelpFunctions.h"

TestHelpFunctions::TestHelpFunctions(SerialOutput* serialOutput): serial{serialOutput} {

}

void TestHelpFunctions::initTests() {
	serial->logSerial("Test TestHelpFunctions: \n");
}

void TestHelpFunctions::runTests() {
    testInt(0);
    testInt(1);
    testInt(2);
    testInt(23847);
    testInt(-34657);
    testInt(234);

    testDouble(0.0);
    testDouble(-10.0);
    testDouble(23.656);
    testDouble(1235.93);
    testDouble(-235.291);
    
    testFloat(0);
    testFloat(-2345);
    testFloat(23446);
    testFloat(98264);

    testBool(true);
    testBool(false);

    testLong(0L);
    testLong(23L);
    testLong(23435L);
    testLong(34546663L);
    testLong(18896723);
    testLong(999978264L);
}   

void TestHelpFunctions::array_to_string(byte array[], unsigned int len, char buffer[])
{
    for (unsigned int i = 0; i < len; i++)
    {
        byte nib1 = (array[i] >> 4) & 0x0F;
        byte nib2 = (array[i] >> 0) & 0x0F;
        buffer[i*2+0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
        buffer[i*2+1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
    }
    buffer[len*2] = '\0';
}

template <typename T>
void TestHelpFunctions::checkByteConvertion(T item) {
  byte buffer[sizeof(T)];
  HelpFunctions::itemToBytes(item, buffer);

  char helpText[sizeof(T)*4];
  array_to_string(buffer, sizeof(T), helpText);
  String text(helpText);
  serial->logSerial(text);
  serial->logSerial("\n");

  T receivedValue;
  HelpFunctions::itemFromBytes(&receivedValue, buffer);

  serial->logSerial("Received: " + String(receivedValue));
  serial->logSerial("\n");
  
  bool equals = item == receivedValue;
  serial->logSerial("Equals: ", equals);
  serial->logSerial("-----------\n" );
}

void TestHelpFunctions::testInt(int value) {
  serial->logSerial("Test Int: " + String(value) + "\n");
  checkByteConvertion(value);
  delay(50);
}

void TestHelpFunctions::testDouble(double value) {
  serial->logSerial("Test Double: " + String(value) + "\n");
  checkByteConvertion(value);
  delay(50);
}

void TestHelpFunctions::testBool(bool value) {
  serial->logSerial("Test Bool: " + String(value) + "\n");
  checkByteConvertion(value);
  delay(50);
}

void TestHelpFunctions::testByte(byte value) {
  serial->logSerial("Test Byte: " + String(value) + "\n");
  checkByteConvertion(value);
  delay(50);
}

void TestHelpFunctions::testFloat(float value) {
  serial->logSerial("Test Float: " + String(value) + "\n");
  checkByteConvertion(value);
  delay(50);
}

void TestHelpFunctions::testLong(long value) {
  serial->logSerial("Test Long: " + String(value) + "\n");
  checkByteConvertion(value);
  delay(50);
}
