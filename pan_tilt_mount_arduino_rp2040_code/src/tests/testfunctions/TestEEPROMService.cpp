#include "TestEEPROMService.h"
#include "../../SerialOutput.h"
#include "../../HelpFunctions.h"
#include <I2C_eeprom.h>

TestEEPROMService::TestEEPROMService(SerialOutput* serialOutput, TwoWire *wire): serial{serialOutput}, eepromService {serialOutput, wire} {

}

void TestEEPROMService::initTests() {
	serial->logSerial("Test TestHelpFunctions: \n");
  serial->logSerial("!Warning: All data in the eeprom get's deleted \n");
  eepromService.initEEPROM();
}

void TestEEPROMService::runTests() {
  if(!eepromService.eepromConnected()) {
    serial->logSerial("NO EEPROM CONNECTED!: \n");
    return;
  }
  serial->logSerial("Run EEPROM Tests \n");
  checkClear();
  testGetPutInt(0);
  testGetPutInt(1);
  testGetPutInt(2);
  testGetPutInt(23847);
  testGetPutInt(-34657);
  testGetPutInt(234);

  testGetPutDouble(0.0);
  testGetPutDouble(-10.0);
  testGetPutDouble(23.656);
  testGetPutDouble(1235.93);
  testGetPutDouble(-235.291);
  
  testGetPutFloat(0);
  testGetPutFloat(-2345);
  testGetPutFloat(23446);
  testGetPutFloat(98264);

  testGetPutBool(true);
  testGetPutBool(false);

  testGetPutLong(0L);
  testGetPutLong(23L);
  testGetPutLong(23435L);
  testGetPutLong(34546663L);
  testGetPutLong(18896723);
  testGetPutLong(999978264L);

  testRead(0x00);
  testRead(0x03);
  testRead(0x10);
  testRead(0xA1);
  testRead(0xFF);

  checkClear();
}   

void TestEEPROMService::checkClear() {
  eepromService.clear();
  bool error = false;
  for(int index = 0; index < deviceSize; index += 1) {
    byte receivedValue = eepromService.read(index);
    if(receivedValue != 0x00) {
      serial->logSerial("Value at Address " + String(receivedValue) + "was not 0 \n");
      error = true;
      break;
    }
  }
  if(error) {
    serial->logSerial("Error: Clear EEPROM not Successful \n");
  } else {
    serial->logSerial("Clear EEPROM not Successful \n");
  }
  serial->logSerial("-----------\n" );
}

template <typename T>
void TestEEPROMService::checkGetPutItem(T item) {
  T saveInputData = item; // If Pointer-Value get's edited
  int eepromAddress = 34; // Random Address
  T receivedPutItem = eepromService.put(eepromAddress, &item);

  T getEepromValue;
  T receivedGetEepromData = eepromService.get(eepromAddress, &getEepromValue);

  serial->logSerial("Input/Output Equals: ", receivedGetEepromData == saveInputData);
  serial->logSerial("Input Equals: ", getEepromValue == receivedGetEepromData);
  serial->logSerial("Output Equals: ", item == receivedPutItem);
  serial->logSerial("Input don't gets edited: ", item == saveInputData);
  serial->logSerial("-----------\n" );
}

void TestEEPROMService::testGetPutInt(int value) {
  serial->logSerial("Test GetPut Int: " + String(value) + "\n");
  checkGetPutItem(value);
  delay(50);
}

void TestEEPROMService::testGetPutDouble(double value) {
  serial->logSerial("Test GetPut Double: " + String(value) + "\n");
  checkGetPutItem(value);
  delay(50);
}

void TestEEPROMService::testGetPutBool(bool value) {
  serial->logSerial("Test GetPut Bool: " + String(value) + "\n");
  checkGetPutItem(value);
  delay(50);
}

void TestEEPROMService::testGetPutByte(byte value) {
  serial->logSerial("Test GetPut Byte: " + String(value) + "\n");
  checkGetPutItem(value);
  delay(50);
}

void TestEEPROMService::testGetPutFloat(float value) {
  serial->logSerial("Test GetPut Float: " + String(value) + "\n");
  checkGetPutItem(value);
  delay(50);
}

void TestEEPROMService::testGetPutLong(long value) {
  serial->logSerial("Test GetPut Long: " + String(value) + "\n");
  checkGetPutItem(value);
  delay(50);
}

void TestEEPROMService::testRead(byte testValue) {
  int eepromAddress = 50; // Random Address
  eepromService.put(eepromAddress, &testValue);

  byte readValue = eepromService.read(eepromAddress);
  serial->logSerial("Equals: ", testValue == readValue);
  serial->logSerial("-----------\n" );
}
