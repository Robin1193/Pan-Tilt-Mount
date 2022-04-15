#ifndef EEPROM_H
#define EEPROM_H

#include <I2C_eeprom.h>
#include "SerialOutput.h"
#include "HelpFunctions.h"

#if ARDUINO > 22
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


class EEPROMService {
public:
    EEPROMService(SerialOutput* serialOutput, TwoWire *wire = &Wire);
    void initEEPROM();
    void clear();

    uint8_t read( int idx );

    template <typename T>
    T get(int idx, T* t) {
        byte buffer[sizeof(T)];
        for(unsigned int index = 0; index < sizeof(T); index += 1) {
            buffer[index] = eeprom.readByte(idx + index);
        }
        HelpFunctions::itemFromBytes(t, buffer);
        T item = *t;
        return item;
    }

    template <typename T>
    T put(int idx, T* t) {
        T item = *t;
        byte buffer[sizeof(T)];
        HelpFunctions::itemToBytes(item, buffer);
        for(unsigned int index = 0; index < sizeof(T); index += 1) {
            eeprom.writeByte(idx + index, buffer[index]);
        }
        return item;
    }

    bool eepromConnected();

private:
    SerialOutput* serialOutput = NULL;
    I2C_eeprom eeprom;

    const int deviceSize = I2C_DEVICESIZE_24LC512;
};

#endif
