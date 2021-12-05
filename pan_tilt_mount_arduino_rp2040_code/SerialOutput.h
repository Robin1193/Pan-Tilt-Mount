#ifndef SERIALOUTPUT_H
#define SERIALOUTPUT_H

#if ARDUINO > 22
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class SerialOutput {
public:
	SerialOutput(int baudRate);
    
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
    void initSerialLog(int baudRate);
};

#endif
