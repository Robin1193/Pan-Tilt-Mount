#include "SerialOutput.h"
#include "GlobalSettings.h"

SerialOutput::SerialOutput() {

}

void SerialOutput::initSerialLog(int baudRate) {
	Serial.begin(baudRate);
}

void SerialOutput::logSerial(String text) {
	if(Serial) {
		Serial.print(text);
	}
}

void SerialOutput::logSerial(String text, char c, String endText) {
	if(Serial) {
		Serial.print(text);
	  Serial.print(c);
	  Serial.print(endText);
	}
}

void SerialOutput::logSerial(short num, String endText) {
	if(Serial) {
		Serial.print(num);
		Serial.print(endText);
	}
}

void SerialOutput::logSerial(unsigned short num, String endText) {
	if(Serial) {
		Serial.print(num);
		Serial.print(endText);	
	}
}

void SerialOutput::logSerial(int num, String endText) {
	if(Serial) {
		Serial.print(num);
		Serial.print(endText);
	}
}

void SerialOutput::logSerial(unsigned int num, String endText) {
	if(Serial) {
		Serial.print(num);
		Serial.print(endText);	
	}
}

void SerialOutput::logSerial(long num, String endText) {
	if(Serial) {
		Serial.print(num);
		Serial.print(endText);
	}
}

void SerialOutput::logSerial(unsigned long num, String endText) {
	if(Serial) {
		Serial.print(num);
		Serial.print(endText);
	}
}

void SerialOutput::logSerial(float num, int dp, String endText) {
	if(Serial) {
		Serial.print(num, dp);
		Serial.print(endText);
	}
}

void SerialOutput::logSerial(double num, int dp, String endText) {
	if(Serial) {
		Serial.print(num, dp);
		Serial.print(endText);
	}
}

void SerialOutput::logSerial(String text, short num, String endText) {
	if(Serial) {
		Serial.print(text);
		Serial.print(num);
		Serial.print(endText);
	}
}

void SerialOutput::logSerial(String text, unsigned short num, String endText) {
	if(Serial) {
		Serial.print(text);
		Serial.print(num);
		Serial.print(endText);
	}
}

void SerialOutput::logSerial(String text, int num, String endText) {
	if(Serial) {
		Serial.print(text);
		Serial.print(num);
		Serial.print(endText);
	}
}

void SerialOutput::logSerial(String text, unsigned int num, String endText) {
	if(Serial) {
		Serial.print(text);
		Serial.print(num);
		Serial.print(endText);
	}
}

void SerialOutput::logSerial(String text, long num, String endText) {
	if(Serial) {
		Serial.print(text);
		Serial.print(num);
		Serial.print(endText);	
	}
}

void SerialOutput::logSerial(String text, unsigned long num, String endText) {
	if(Serial) {
		Serial.print(text);
		Serial.print(num);
		Serial.print(endText);
	}
}

void SerialOutput::logSerial(String text, float num, int dp, String endText) {
	if(Serial) {
		Serial.print(text);
		Serial.print(num, dp);
		Serial.print(endText);	
	}
}

void SerialOutput::logSerial(String text, double num, int dp, String endText) {
	if(Serial) {
		Serial.print(text);
		Serial.print(num, dp);
		Serial.print(endText);	
	}
}

void SerialOutput::serialFlush() {
	if(Serial) {
		while(dataAvailable()){
        	Serial.read();
    	}
	}
}

String SerialOutput::serialData() {
	memset(&stringText[0], 0, sizeof(stringText)); //clear the array
	while(dataAvailable()){//set elemetns of stringText to the serial values sent
		char digit = Serial.read(); //read in a char
		strncat(stringText, &digit, 1); //add digit to the end of the array
	}
	serialFlush();//Clear any excess data in the serial buffer

	return stringText;
}

bool SerialOutput::dataAvailable() {
	return Serial && Serial.available();
}
