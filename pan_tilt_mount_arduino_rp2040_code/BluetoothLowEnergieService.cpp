#include "BluetoothLowEnergieService.h"
#include "SerialOutput.h"
#include "GlobalSettings.h"

#include <ArduinoBLE.h>

BluetoothLowEnergieService::BluetoothLowEnergieService(SerialOutput* output) {
    serialOutput = output;
    bleGeneralInformationService = BLEService(BLE_GENERAL_INFO_SERVICE_UUID);
    bleSerialCommandService = BLEService(BLE_SERIAL_COMMAND_SERVICE_UUID);

    bleGeneralInfoBleApiVersionCharacteristic = BLECharacteristic(BLE_GENERAL_INFO_BLE_API_VERSION_UUID, BLERead, BLE_API_VERSION);
    bleGeneralInfoSoftwareVersionCharacteristic = BLECharacteristic(BLE_GENERAL_INFO_SOFTWARE_VERSION_UUID, BLERead, VERSION_NUMBER);
    bleGeneralInfoDeviceNameCharacteristic = BLECharacteristic(BLE_GENERAL_INFO_DEVICE_NAME_UUID, BLERead, BLE_DEVICE_NAME);

    bleSerialCommandFromMobileDeviceService = BLECharacteristic(BLE_SERIAL_COMMAND_COMMAND_FROM_MOBILE_DEVICE_UUID, BLEWrite, 20, false);
    bleSerialCommandToMobileDeviceService = BLECharacteristic(BLE_SERIAL_COMMAND_COMMAND_TO_MOBILE_DEVICE_UUID, BLERead | BLENotify, 50, false);

    peripheral = BLEDevice();
}

void BluetoothLowEnergieService::initBle() {
    if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
  }

  BLE.setDeviceName(BLE_DEVICE_NAME);
  BLE.setLocalName(BLE_DEVICE_NAME);
  BLE.setAdvertisedService(bleGeneralInformationService);
  BLE.setAdvertisedService(bleSerialCommandService);

  bleGeneralInformationService.addCharacteristic(bleGeneralInfoBleApiVersionCharacteristic);
  bleGeneralInformationService.addCharacteristic(bleGeneralInfoSoftwareVersionCharacteristic);
  bleGeneralInformationService.addCharacteristic(bleGeneralInfoDeviceNameCharacteristic);

  bleSerialCommandService.addCharacteristic(bleSerialCommandFromMobileDeviceService);
  bleSerialCommandService.addCharacteristic(bleSerialCommandToMobileDeviceService);

  BLE.addService(bleGeneralInformationService);
  BLE.addService(bleSerialCommandService);

  BLE.advertise();
}

bool BluetoothLowEnergieService::isBleConnected() {
  peripheral = BLE.central();
    if(peripheral) {
      if(!bleConnected) {
        serialOutput->logSerial("Connected to central: ");
        serialOutput->logSerial(peripheral.address());
      }
      bleConnected = true;
      return true;
    } else {
      if(bleConnected) {
        serialOutput->logSerial("BLE Disconnected.");
      }
      bleConnected = false;
      return false;
    }  
}

bool BluetoothLowEnergieService::wasSerialCommandFromMobileDeviceReceived() {
    return bleSerialCommandFromMobileDeviceService.written();
}

String BluetoothLowEnergieService::getSerialCommandFromMobileDevice() {
    memset(&bleSerialCommandFromMobileDevice[0], 0, sizeof(bleSerialCommandFromMobileDevice)); //clear the array
    bleSerialCommandFromMobileDeviceService.readValue(bleSerialCommandFromMobileDevice, 20);
    

    String serialCommand(bleSerialCommandFromMobileDevice);
    serialOutput->logSerial("Written Value: ");
    serialOutput->logSerial(serialCommand);
    return String(serialCommand);
}
