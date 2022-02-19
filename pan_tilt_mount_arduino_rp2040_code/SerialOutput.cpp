#include "SerialOutput.h"
#include "GlobalSettings.h"

SerialOutput::SerialOutput() {

}

SerialOutput::SerialOutput(int baudRate) {
    initSerialLog(baudRate);
}

void SerialOutput::initSerialLog(int baudRate) {
    //Serial.begin(baudRate);
}

void SerialOutput::logSerial(String text) {
	/*
	if(Serial) {
		Serial.print(text);
	}
	*/
}

void SerialOutput::logSerial(String text, char c, String endText) {
	/*
	if(Serial) {
		Serial.print(text);
	  Serial.print(c);
	  Serial.print(endText);
	}
	*/
}

void SerialOutput::logSerial(short num, String endText) {
	/*
	if(Serial) {
		Serial.print(num);
		Serial.print(endText);
	}
	*/
}

void SerialOutput::logSerial(unsigned short num, String endText) {
	/*
	if(Serial) {
		Serial.print(num);
		Serial.print(endText);	
	}
	*/
}

void SerialOutput::logSerial(int num, String endText) {
	/*
	if(Serial) {
		Serial.print(num);
		Serial.print(endText);
	}
	*/
}

void SerialOutput::logSerial(unsigned int num, String endText) {
	/*
	if(Serial) {
		Serial.print(num);
		Serial.print(endText);	
	}
	*/
}

void SerialOutput::logSerial(long num, String endText) {
	/*
	if(Serial) {
		Serial.print(num);
		Serial.print(endText);
	}
	*/
}

void SerialOutput::logSerial(unsigned long num, String endText) {
	/*
	if(Serial) {
		Serial.print(num);
		Serial.print(endText);
	}
	*/
}

void SerialOutput::logSerial(float num, int dp, String endText) {
	/*
	if(Serial) {
		Serial.print(num, dp);
		Serial.print(endText);
	}
	*/
}

void SerialOutput::logSerial(double num, int dp, String endText) {
	/*
	if(Serial) {
		Serial.print(num, dp);
		Serial.print(endText);
	}
	*/
}

void SerialOutput::logSerial(String text, short num, String endText) {
	/*
	if(Serial) {
		Serial.print(text);
		Serial.print(num);
		Serial.print(endText);
	}
	*/
}

void SerialOutput::logSerial(String text, unsigned short num, String endText) {
	/*
	if(Serial) {
		Serial.print(text);
		Serial.print(num);
		Serial.print(endText);
	}
	*/
}

void SerialOutput::logSerial(String text, int num, String endText) {
	/*
	if(Serial) {
		Serial.print(text);
		Serial.print(num);
		Serial.print(endText);
	}
	*/
}

void SerialOutput::logSerial(String text, unsigned int num, String endText) {
	/*
	if(Serial) {
		Serial.print(text);
		Serial.print(num);
		Serial.print(endText);
	}
	*/
}

void SerialOutput::logSerial(String text, long num, String endText) {
	/*
	if(Serial) {
		Serial.print(text);
		Serial.print(num);
		Serial.print(endText);	
	}
	*/
}

void SerialOutput::logSerial(String text, unsigned long num, String endText) {
	/*
	if(Serial) {
		Serial.print(text);
		Serial.print(num);
		Serial.print(endText);
	}
	*/
}

void SerialOutput::logSerial(String text, float num, int dp, String endText) {
	/*
	if(Serial) {
		Serial.print(text);
		Serial.print(num, dp);
		Serial.print(endText);	
	}
	*/
}

void SerialOutput::logSerial(String text, double num, int dp, String endText) {
	/*
	if(Serial) {
		Serial.print(text);
		Serial.print(num, dp);
		Serial.print(endText);	
	}
	*/
}

void SerialOutput::serialFlush() {
	/*
	if(Serial) {
		while(Serial.available() > 0){
        	Serial.read();
    	}
	}
	*/
}

void SerialOutput::serialData() {
	/*
	void serialData(void){
		char instruction = Serial.read();
		if(instruction == INSTRUCTION_BYTES_SLIDER_PAN_TILT_SPEED){
			int count = 0;
			while(Serial.available() < 6){//Wait for 6 bytes to be available. Breaks after ~20ms if bytes are not received.
					delayMicroseconds(200); 
					count++;
					if(count > 100){
						serialFlush();//Clear the serial buffer
						break;   
					}
				}
				int sliderStepSpeed = (Serial.read() << 8) + Serial.read(); 
				int panStepSpeed = (Serial.read() << 8) + Serial.read(); 
				int tiltStepSpeed = (Serial.read() << 8) + Serial.read(); 

				stepper_slider.setSpeed(sliderStepSpeed);
				stepper_pan.setSpeed(panStepSpeed);
				stepper_tilt.setSpeed(tiltStepSpeed);
				stepper_slider.runSpeed();
				stepper_pan.runSpeed();
				stepper_tilt.runSpeed();
		}
		
		delay(2); //wait to make sure all data in the serial message has arived 
		memset(&stringText[0], 0, sizeof(stringText)); //clear the array
		while(Serial.available()){//set elemetns of stringText to the serial values sent
			char digit = Serial.read(); //read in a char
			strncat(stringText, &digit, 1); //add digit to the end of the array
		}
		serialFlush();//Clear any excess data in the serial buffer
		int serialCommandValueInt = atoi(stringText); //converts stringText to an int
		float serialCommandValueFloat = atof(stringText); //converts stringText to a float
		processCommand(instruction, serialCommandValueInt, serialCommandValueFloat);
	}
	*/
}

bool SerialOutput::available() {
	return false;
	//return Serial && Serial.available();
}
