#include "BluetoothLowEnergieService.h"
#include "SerialOutput.h"
#include "GlobalSettings.h"

#include <ArduinoBLE.h>

BluetoothLowEnergieService::BluetoothLowEnergieService() {

}

BluetoothLowEnergieService::BluetoothLowEnergieService(SerialOutput* output): serialOutput{output} {
   ;
}

void BluetoothLowEnergieService::initBle() {
    if (!BLE.begin()) {
      /*
      if(serialOutput) {
        serialOutput->logSerial("starting BLE failed!");
      }
      */
     ;
  } else {
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

}

bool BluetoothLowEnergieService::isBleConnected() {  
  peripheral = BLE.central();
    if(peripheral) {
      if(!bleConnected) {
        /*
        if(serialOutput) {
          serialOutput->logSerial("Connected to central: ");
          serialOutput->logSerial(peripheral.address());
        }
        */
      }
      bleConnected = true;
      return true;
    } else {
      if(bleConnected) {
        /*
        if(serialOutput) {
          serialOutput->logSerial("BLE Disconnected.");
        }
        */
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
    /*
    if(serialOutput) {
      serialOutput->logSerial("Written Value: ");
      serialOutput->logSerial(serialCommand);
    }
    */
    return String(serialCommand);
}


void BluetoothLowEnergieService::logSerial(String text) {
  //writeSerialOutput(text);
}

void BluetoothLowEnergieService::logSerial(String text, char c, String endText) {
	//writeSerialOutput(text + " " + c + " " + endText);
}

void BluetoothLowEnergieService::logSerial(short num, String endText) {
	//writeSerialOutput(String(num) + " " + endText);
}

void BluetoothLowEnergieService::logSerial(unsigned short num, String endText) {
	//writeSerialOutput(String(num) + " " + endText);
}

void BluetoothLowEnergieService::logSerial(int num, String endText) {
	//writeSerialOutput(String(num) + " " + endText);
}

void BluetoothLowEnergieService::logSerial(unsigned int num, String endText) {
	//writeSerialOutput(String(num) + " " + endText);
}

void BluetoothLowEnergieService::logSerial(long num, String endText) {
	//writeSerialOutput(String(num) + " " + endText);
}

void BluetoothLowEnergieService::logSerial(unsigned long num, String endText) {
	//writeSerialOutput(String(num) + " " + endText);
}

void BluetoothLowEnergieService::logSerial(float num, int dp, String endText) {
	//writeSerialOutput(String(num) + " " + String(dp) + " " + endText);
}

void BluetoothLowEnergieService::logSerial(double num, int dp, String endText) {
	//writeSerialOutput(String(num) + " " + String(dp) + " " + endText);
}

void BluetoothLowEnergieService::logSerial(String text, short num, String endText) {
	//writeSerialOutput(text + String(num) + " " + endText);
}

void BluetoothLowEnergieService::logSerial(String text, unsigned short num, String endText) {
	//writeSerialOutput(text + String(num) + " " + endText);
}

void BluetoothLowEnergieService::logSerial(String text, int num, String endText) {
	//writeSerialOutput(text + String(num) + " " + endText);
}

void BluetoothLowEnergieService::logSerial(String text, unsigned int num, String endText) {
  //writeSerialOutput(text + String(num) + " " + endText);
}

void BluetoothLowEnergieService::logSerial(String text, long num, String endText) {
  //writeSerialOutput(text + String(num) + " " + endText);
}

void BluetoothLowEnergieService::logSerial(String text, unsigned long num, String endText) {
  //writeSerialOutput(text + String(num) + " " + endText);
}

void BluetoothLowEnergieService::logSerial(String text, float num, int dp, String endText) {
  //writeSerialOutput(text + String(num) + " " + endText);
}

void BluetoothLowEnergieService::logSerial(String text, double num, int dp, String endText) {
    //writeSerialOutput(text + String(num) + " " + endText);
}

void BluetoothLowEnergieService::writeSerialOutput(String text) {
  /*
  if(isBleConnected()) {
    //bleSerialCommandToMobileDeviceService.writeValue(text.c_str(), text.length());
  }
  */
}
