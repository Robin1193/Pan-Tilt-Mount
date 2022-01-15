#ifndef PANTILTMOUNT_H
#define PANTILTMOUNT_H

struct KeyframeElement {
    long panStepCount = 0;
    float panSpeed = 0;
    long tiltStepCount = 0;
    float tiltSpeed = 0;
    long sliderStepCount = 0;
    float sliderSpeed = 0;
    int msDelay = 0;
};

struct FloatCoordinate {
    float x;
    float y;
    float z;
};

struct LinePoints {
    float x0;
    float y0;
    float x1;
    float y1;
};

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

void initPanTilt(void);
void serialFlush(void);
void enableSteppers(void);
void setStepMode(int);
void serialData(void);
void mainLoop(void);
void panDegrees(float);
void tiltDegrees(float);
void debugReport(void);
bool findHome(void);
float getBatteryVoltage(void);
float getBatteryPercentage(void);
float boundFloat(float, float, float);
float panDegreesToSteps(float);
float tiltDegreesToSteps(float);
float panStepsToDegrees(long);
float panStepsToDegrees(float);
float tiltStepsToDegrees(long);
float tiltStepsToDegrees(float);
int addPosition(void);
void clearKeyframes(void);
void executeMoves(int);
void moveToIndex(int);
void gotoFirstKeyframe(void);
void gotoLastKeyframe(void);
void editKeyframe(void);
void addDelay(unsigned int ms);
void editDelay(unsigned int ms);
void printKeyframeElements(void);
void saveEEPROM(void);
void printEEPROM(void);
void setEEPROMVariables(void);
void invertPanDirection(bool);
void invertTiltDirection(bool);
int setTargetPositions(float, float, float);
void toggleAutoHoming(void);
void triggerCameraShutter(void);
void panoramiclapseInterpolation(float, float, float, float, float, float, float, unsigned long);
void panoramiclapse(float, unsigned long, int);
long sliderMillimetresToSteps(float);
float sliderStepsToMillimetres(long);
void sliderMoveTo(float);
void invertSliderDirection(bool);
void timelapse(unsigned int, unsigned long);
bool calculateTargetCoordinate(void);
void interpolateTargetPoint(FloatCoordinate);
void toggleAcceleration(void);
void scaleKeyframeSpeed(float);
void processCommand(char, int, float);
void checkBle();
//void writeLogToBle(String);
//void writeLog(String);

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

#endif
