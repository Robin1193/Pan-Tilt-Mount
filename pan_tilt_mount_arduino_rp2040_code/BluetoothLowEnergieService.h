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


    // Write "Serial"-Output to BLE
    void logSerial(String text);
    void logSerial(String text, char c, String endText = "\n");
    void logSerial(short num, String endText = "\n");
    void logSerial(unsigned short num, String endText = "\n");
    void logSerial(int num, String endText = "\n");
    void logSerial(unsigned int num, String endText = "\n");
    void logSerial(long num, String endText = "\n");
    void logSerial(unsigned long num, String endText = "\n");
    void logSerial(float num, int dp = 3, String endText = "\n");
    void logSerial(double num, int dp = 3, String endText = "\n");
    void logSerial(String text, short num, String endText = "\n");
    void logSerial(String text, unsigned short num, String endText = "\n");
    void logSerial(String text, int num, String endText = "\n");
    void logSerial(String text, unsigned int num, String endText = "\n");
    void logSerial(String text, long num, String endText = "\n");
    void logSerial(String text, unsigned long num, String endText = "\n");
    void logSerial(String text, float num, int dp = 3, String endText = "\n");
    void logSerial(String text, double num, int dp = 3, String endText = "\n");

private:
    BluetoothLowEnergieService();
    
    BLEService bleGeneralInformationService{BLE_GENERAL_INFO_SERVICE_UUID};
    BLEService bleSerialCommandService{BLE_SERIAL_COMMAND_SERVICE_UUID};

    BLECharacteristic bleGeneralInfoBleApiVersionCharacteristic{BLE_GENERAL_INFO_BLE_API_VERSION_UUID, BLERead, BLE_API_VERSION};
    BLECharacteristic bleGeneralInfoSoftwareVersionCharacteristic{BLE_GENERAL_INFO_SOFTWARE_VERSION_UUID, BLERead, VERSION_NUMBER};
    BLECharacteristic bleGeneralInfoDeviceNameCharacteristic{BLE_GENERAL_INFO_DEVICE_NAME_UUID, BLERead, BLE_DEVICE_NAME};

    BLECharacteristic bleSerialCommandFromMobileDeviceService{BLE_SERIAL_COMMAND_COMMAND_FROM_MOBILE_DEVICE_UUID, BLEWrite, 20, false};
    BLECharacteristic bleSerialCommandToMobileDeviceService{BLE_SERIAL_COMMAND_COMMAND_TO_MOBILE_DEVICE_UUID, BLERead | BLENotify, 120, false};

    BLEDevice peripheral;

    char bleSerialCommandFromMobileDevice[20];
    bool bleConnected = false;
    SerialOutput* serialOutput = NULL;

    void writeSerialOutput(String text);
};

#endif
