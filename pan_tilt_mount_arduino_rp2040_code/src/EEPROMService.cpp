#include "EEPROMService.h"

EEPROMService::EEPROMService(SerialOutput* serialOutput, TwoWire *wire): serialOutput{serialOutput}, eeprom {0x50, I2C_DEVICESIZE_24LC512, wire} {

}

void EEPROMService::initEEPROM() {
    eeprom.begin();
    if(!eepromConnected())
    {
        serialOutput->logSerial("ERROR: Can't find eeprom...");
    }
}

void EEPROMService::clear() {
    for(int i = 0; i < deviceSize; i += 1) {
        eeprom.writeByte(i, 0);
    }
}

bool EEPROMService::eepromConnected() {
    return eeprom.isConnected();
}

uint8_t EEPROMService::read(int idx ) {
    return eeprom.readByte(idx);
}
