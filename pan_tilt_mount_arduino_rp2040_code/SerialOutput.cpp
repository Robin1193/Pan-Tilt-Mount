#include "SerialOutput.h"

SerialOutput::SerialOutput(int baudRate) {
    initSerialLog(baudRate);
}

void SerialOutput::initSerialLog(int baudRate) {
    Serial.begin(baudRate);
}

void SerialOutput::logSerial(String text) {
	Serial.print(text);
}

void SerialOutput::logSerial(String text, char c, String endText) {
	Serial.print(text);
	Serial.print(c);
	Serial.print(endText);
}

void SerialOutput::logSerial(short num, String endText) {
	Serial.print(num);
	Serial.print(endText);
}

void SerialOutput::logSerial(unsigned short num, String endText) {
	Serial.print(num);
	Serial.print(endText);
}

void SerialOutput::logSerial(int num, String endText) {
	Serial.print(num);
	Serial.print(endText);
}

void SerialOutput::logSerial(unsigned int num, String endText) {
	Serial.print(num);
	Serial.print(endText);
}

void SerialOutput::logSerial(long num, String endText) {
	Serial.print(num);
	Serial.print(endText);
}

void SerialOutput::logSerial(unsigned long num, String endText) {
	Serial.print(num);
	Serial.print(endText);
}

void SerialOutput::logSerial(float num, int dp, String endText) {
	Serial.print(num, dp);
	Serial.print(endText);
}

void SerialOutput::logSerial(double num, int dp, String endText) {
	Serial.print(num, dp);
	Serial.print(endText);
}

void SerialOutput::logSerial(String text, short num, String endText) {
	Serial.print(text);
	Serial.print(num);
	Serial.print(endText);
}

void SerialOutput::logSerial(String text, unsigned short num, String endText) {
	Serial.print(text);
	Serial.print(num);
	Serial.print(endText);
}

void SerialOutput::logSerial(String text, int num, String endText) {
	Serial.print(text);
	Serial.print(num);
	Serial.print(endText);
}

void SerialOutput::logSerial(String text, unsigned int num, String endText) {
	Serial.print(text);
	Serial.print(num);
	Serial.print(endText);
}

void SerialOutput::logSerial(String text, long num, String endText) {
	Serial.print(text);
	Serial.print(num);
	Serial.print(endText);
}

void SerialOutput::logSerial(String text, unsigned long num, String endText) {
	Serial.print(text);
	Serial.print(num);
	Serial.print(endText);
}

void SerialOutput::logSerial(String text, float num, int dp, String endText) {
	Serial.print(text);
	Serial.print(num, dp);
	Serial.print(endText);
}

void SerialOutput::logSerial(String text, double num, int dp, String endText) {
	Serial.print(text);
	Serial.print(num, dp);
	Serial.print(endText);
}
