#ifndef Log_H
#define Log_H

#include "SerialOutput.h"
#include "BluetoothLowEnergieService.h"

#if ARDUINO > 22
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class Log {
public:
	Log(
        SerialOutput* serialOutputItem, 
        BluetoothLowEnergieService* bleServiceItem
    );
    
    void log(String text, bool serialOnly = false);
    void log(String text, char c, String endText = "\n", bool serialOnly = false);
    void log(short num, String endText = "\n", bool serialOnly = false);
    void log(unsigned short num, String endText = "\n", bool serialOnly = false);
    void log(int num, String endText = "\n", bool serialOnly = false);
    void log(unsigned int num, String endText = "\n", bool serialOnly = false);
    void log(long num, String endText = "\n", bool serialOnly = false);
    void log(unsigned long num, String endText = "\n", bool serialOnly = false);
    void log(float num, int dp = 3, String endText = "\n", bool serialOnly = false);
    void log(double num, int dp = 3, String endText = "\n", bool serialOnly = false);
    void log(String text, short num, String endText = "\n", bool serialOnly = false);
    void log(String text, unsigned short num, String endText = "\n", bool serialOnly = false);
    void log(String text, int num, String endText = "\n", bool serialOnly = false);
    void log(String text, unsigned int num, String endText = "\n", bool serialOnly = false);
    void log(String text, long num, String endText = "\n", bool serialOnly = false);
    void log(String text, unsigned long num, String endText = "\n", bool serialOnly = false);
    void log(String text, float num, int dp = 3, String endText = "\n", bool serialOnly = false);
    void log(String text, double num, int dp = 3, String endText = "\n", bool serialOnly = false);

private:
    Log();
    SerialOutput* serialOutput = NULL;
    BluetoothLowEnergieService* bleService = NULL;

};

#endif
