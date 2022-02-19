#include "Log.h"

#include "SerialOutput.h"
#include "BluetoothLowEnergieService.h"

Log::Log() {

}

Log::Log(
	SerialOutput* serialOutputItem, 
    BluetoothLowEnergieService* bleServiceItem
): serialOutput{serialOutputItem}, bleService{bleServiceItem} {
}

void Log::log(String text) {
	if(serialOutput) {
		serialOutput->logSerial(text);
	}
	if(bleService) {
		bleService->logSerial(text);
	}
}

void Log::log(String text, char c, String endText) {
	if(serialOutput) {
		serialOutput->logSerial(text, c, endText);
	}
	if(bleService) {
		bleService->logSerial(text, c, endText);
	}
}

void Log::log(short num, String endText) {
	if(serialOutput) {
		serialOutput->logSerial(num, endText);
	}
	if(bleService) {
		bleService->logSerial(num, endText);	
	}
}

void Log::log(unsigned short num, String endText) {
	if(serialOutput) {
		serialOutput->logSerial(num, endText);
	}
	if(bleService) {
		bleService->logSerial(num, endText);
	}
}

void Log::log(int num, String endText) {
	if(serialOutput) {
		serialOutput->logSerial(num, endText);
	}
	if(bleService) {
		bleService->logSerial(num, endText);
	}
}

void Log::log(unsigned int num, String endText) {
	if(serialOutput) {
		serialOutput->logSerial(num, endText);
	}
	if(bleService) {
		bleService->logSerial(num, endText);
	}
}

void Log::log(long num, String endText) {
	if(serialOutput) {
		serialOutput->logSerial(num, endText);
	}
	if(bleService) {
		bleService->logSerial(num, endText);
	}
}

void Log::log(unsigned long num, String endText) {
	if(serialOutput) {
		serialOutput->logSerial(num, endText);
	}
	if(bleService) {
		bleService->logSerial(num, endText);
	}
}

void Log::log(float num, int dp, String endText) {
	if(serialOutput) {
		serialOutput->logSerial(num, dp, endText);
	}
	if(bleService) {
		bleService->logSerial(num, dp, endText);
	}
}

void Log::log(double num, int dp, String endText) {
	if(serialOutput) {
		serialOutput->logSerial(num, dp, endText);
	}
	if(bleService) {
		bleService->logSerial(num, dp, endText);
	}
}

void Log::log(String text, short num, String endText) {
	if(serialOutput) {
		serialOutput->logSerial(text, num, endText);
	}
	if(bleService) {
		bleService->logSerial(text, num, endText);
	}
}

void Log::log(String text, unsigned short num, String endText) {
	if(serialOutput) {
		serialOutput->logSerial(text, num, endText);
	}
	if(bleService) {
		bleService->logSerial(text, num, endText);
	}
}

void Log::log(String text, int num, String endText) {
	if(serialOutput) {
		serialOutput->logSerial(text, num, endText);
	}
	if(bleService) {
		bleService->logSerial(text, num, endText);
	}
}

void Log::log(String text, unsigned int num, String endText) {
	if(serialOutput) {
		serialOutput->logSerial(text, num, endText);
	}
	if(bleService) {
		bleService->logSerial(text, num, endText);
	}
}

void Log::log(String text, long num, String endText) {
	if(serialOutput) {
		serialOutput->logSerial(text, num, endText);
	}
	if(bleService) {
		bleService->logSerial(text, num, endText);
	}
}

void Log::log(String text, unsigned long num, String endText) {
	if(serialOutput) {
		serialOutput->logSerial(text, num, endText);
	}
	if(bleService) {
		bleService->logSerial(text, num, endText);
	}
}

void Log::log(String text, float num, int dp, String endText) {
	if(serialOutput) {
		serialOutput->logSerial(text, num, dp, endText);
	}
	if(bleService) {
		bleService->logSerial(text, num, dp, endText);
	}
}

void Log::log(String text, double num, int dp, String endText) {
	if(serialOutput) {
		serialOutput->logSerial(text, num, dp, endText);
	}
	if(bleService) {
		bleService->logSerial(text, num, dp, endText);
	}
}
