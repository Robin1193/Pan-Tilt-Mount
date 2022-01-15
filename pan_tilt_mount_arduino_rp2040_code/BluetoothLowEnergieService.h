#ifndef BLUETOOTHLOWENERGIESERVICE_H
#define BLUETOOTHLOWENERGIESERVICE_H

#include "GlobalSettings.h"
#include "SerialOutput.h"
#include <ArduinoBLE.h>

class BluetoothLowEnergieService {
public: 
    BluetoothLowEnergieService(SerialOutput* output);
    void initBle();
    bool isBleConnected();

    bool wasSerialCommandFromMobileDeviceReceived();
    String getSerialCommandFromMobileDevice();

private:
    BLEService bleGeneralInformationService;
    BLEService bleSerialCommandService;

    BLECharacteristic bleGeneralInfoBleApiVersionCharacteristic;
    BLECharacteristic bleGeneralInfoSoftwareVersionCharacteristic;
    BLECharacteristic bleGeneralInfoDeviceNameCharacteristic;

    BLECharacteristic bleSerialCommandFromMobileDeviceService;
    BLECharacteristic bleSerialCommandToMobileDeviceService;

    BLEDevice peripheral;

    char bleSerialCommandFromMobileDevice[20];
    bool bleConnected = false;
    SerialOutput* serialOutput;
};

#endif
