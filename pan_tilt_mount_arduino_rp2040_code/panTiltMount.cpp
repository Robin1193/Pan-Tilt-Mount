#include "PanTiltMount.h"
#include "GlobalSettings.h"
#include "SerialOutput.h"
#include "Log.h"
#include "BluetoothLowEnergieService.h"
#include <AccelStepper.h> //Library to control the stepper motors http://www.airspayce.com/mikem/arduino/AccelStepper/index.html
#include <MultiStepper.h> //Library to control multiple coordinated stepper motors http://www.airspayce.com/mikem/arduino/AccelStepper/classMultiStepper.html#details
//#include <EEPROM.h> //To be able to save values when powered off
#include <WiFiNINA.h>
#include "HelpFunctions.h"


/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

//Global scope
AccelStepper stepper_pan = AccelStepper(1, PIN_STEP_PAN, PIN_DIRECTION_PAN);
AccelStepper stepper_tilt = AccelStepper(1, PIN_STEP_TILT, PIN_DIRECTION_TILT);
AccelStepper stepper_slider = AccelStepper(1, PIN_STEP_SLIDER, PIN_DIRECTION_SLIDER);

MultiStepper multi_stepper;

KeyframeElement keyframe_array[KEYFRAME_ARRAY_LENGTH];

SerialOutput serialOutput;
BluetoothLowEnergieService ble(&serialOutput);
Log logger(&serialOutput, &ble);

int keyframe_elements = 0;
int current_keyframe_index = -1;
char stringText[MAX_STRING_LENGTH + 1];
float pan_steps_per_degree = (200.0 * SIXTEENTH_STEP * PAN_GEAR_RATIO) / 360.0; //Stepper motor has 200 steps per 360 degrees
float tilt_steps_per_degree = (200.0 * SIXTEENTH_STEP * TILT_GEAR_RATIO) / 360.0; //Stepper motor has 200 steps per 360 degrees
float slider_steps_per_millimetre = (200.0 * SIXTEENTH_STEP) / (SLIDER_PULLEY_TEETH * 2); //Stepper motor has 200 steps per 360 degrees, the timing pully has 36 teeth and the belt has a pitch of 2mm

int step_mode = SIXTEENTH_STEP;
bool enable_state = true; //Stepper motor driver enable state
float hall_pan_offset_degrees = 0; //Offset to make the pan axis home position centred. This is required because the Hall sensor triggers before being centred on the magnet.
float hall_tilt_offset_degrees = 0; //Offset to make the tilt axis home position centred. This is required because the Hall sensor triggers before being centred on the magnet.
byte invert_pan = 0; //Variables to invert the direction of the axis. Note: These value gets set from the saved EEPROM value on startup. 
byte invert_tilt = 0;
byte invert_slider = 0;
byte homing_mode = 0; //Note: Gets set from the saved EEPROM value on startup 
float pan_max_speed = 18; //degrees/second. Note: Gets set from the saved EEPROM value on startup. 
float tilt_max_speed = 10; //degrees/second.
float slider_max_speed = 20; //mm/second
long target_position[3]; //Array to store stepper motor step counts
float degrees_per_picture = 0.5; //Note: Gets set from the saved EEPROM value on startup. 
unsigned long delay_ms_between_pictures = 1000; //Note: Gets set from the saved EEPROM value on startup. 
int pan_accel_increment_us = 4000;
int tilt_accel_increment_us = 3000; 
int slider_accel_increment_us = 3500; 
byte acceleration_enable_state = 0;
FloatCoordinate intercept;

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

void initPanTilt(void){
    serialOutput.initSerialLog(BAUD_RATE);
    ble.initBle();
    pinMode(PIN_MS1, OUTPUT);
    pinMode(PIN_MS2, OUTPUT);
    pinMode(PIN_MS3, OUTPUT);
    pinMode(PIN_ENABLE, OUTPUT);
    pinMode(PIN_DIRECTION_PAN, OUTPUT);
    pinMode(PIN_STEP_PAN, OUTPUT);
    pinMode(PIN_DIRECTION_TILT, OUTPUT);
    pinMode(PIN_STEP_TILT, OUTPUT);
    pinMode(PIN_DIRECTION_SLIDER, OUTPUT);
    pinMode(PIN_STEP_SLIDER, OUTPUT);
  
    pinMode(PIN_PAN_HALL, INPUT_PULLUP);
    pinMode(PIN_TILT_HALL, INPUT_PULLUP);
    pinMode(PIN_SLIDER_HALL, INPUT_PULLUP);
    pinMode(PIN_SHUTTER_TRIGGER, OUTPUT);
    digitalWrite(PIN_SHUTTER_TRIGGER, LOW);
    //setEEPROMVariables();

    setStepMode(step_mode); //steping mode
    stepper_pan.setMaxSpeed(panDegreesToSteps(pan_max_speed));
    stepper_tilt.setMaxSpeed(tiltDegreesToSteps(tilt_max_speed));
    stepper_slider.setMaxSpeed(sliderMillimetresToSteps(slider_max_speed));
    stepper_pan.setAcceleration(5000);
    stepper_tilt.setAcceleration(5000);
    stepper_slider.setAcceleration(5000);

    invertPanDirection(invert_pan);
    invertTiltDirection(invert_tilt);
    invertSliderDirection(invert_slider);

    multi_stepper.addStepper(stepper_pan);
    multi_stepper.addStepper(stepper_tilt);
    multi_stepper.addStepper(stepper_slider);

    digitalWrite(PIN_ENABLE, LOW); //Enable the stepper drivers
//    if(homing_mode == 1){
//        logger.log(F("Homing\n"));
//        if(findHome()){
//            logger.log(F("Complete\n"));
//        }
//        else{
//            stepper_pan.setCurrentPosition(0);
//            stepper_tilt.setCurrentPosition(0);
//            logger.log(F("Error homing\n"));
//        }
//    }
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

float boundFloat(float value, float lower, float upper){
    if(value < lower){
        value = lower;
    }
    else if(value > upper){
        value = upper;
    }
    return value;
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

void enableSteppers(void){
    if(enable_state == false){
        digitalWrite(PIN_ENABLE, LOW); //Enable the stepper drivers
        enable_state = true;
        logger.log("Enabled\n");
    } else {
        digitalWrite(PIN_ENABLE, HIGH); //Disabe the stepper drivers
        enable_state = false;
        logger.log("Disabled\n");
    }
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

void setStepMode(int newMode){ //Step modes for the TMC2208
    float stepRatio = (float)newMode / (float)step_mode; //Ratio between the new step mode and the previously set one. 
    
    if(newMode == HALF_STEP){
        digitalWrite(PIN_MS1, HIGH);
        digitalWrite(PIN_MS2, LOW);
        digitalWrite(PIN_MS3, LOW);
    }
    else if(newMode == QUARTER_STEP){
        digitalWrite(PIN_MS1, LOW);
        digitalWrite(PIN_MS2, HIGH);
        digitalWrite(PIN_MS3, LOW);
    }
    else if(newMode == EIGHTH_STEP){
        digitalWrite(PIN_MS1, HIGH);
        digitalWrite(PIN_MS2, HIGH);
        digitalWrite(PIN_MS3, LOW);
    }
    else if(newMode == SIXTEENTH_STEP){
        digitalWrite(PIN_MS1, HIGH);
        digitalWrite(PIN_MS2, HIGH);
        digitalWrite(PIN_MS3, HIGH);
    }
    else{ //If an invalid step mode was entered.
        logger.log("Invalid mode. Enter 2, 4, 8 or 16\n");
        return;
    }
    
    //Scale current step to match the new step mode
    stepper_pan.setCurrentPosition(stepper_pan.currentPosition() * stepRatio);
    stepper_tilt.setCurrentPosition(stepper_tilt.currentPosition() * stepRatio);
    stepper_slider.setCurrentPosition(stepper_slider.currentPosition() * stepRatio);

    pan_steps_per_degree = (200.0 * (float)newMode * PAN_GEAR_RATIO) / 360.0; //Stepper motor has 200 steps per 360 degrees
    tilt_steps_per_degree = (200.0 * (float)newMode * TILT_GEAR_RATIO) / 360.0; //Stepper motor has 200 steps per 360 degrees
    slider_steps_per_millimetre = (200.0 * (float)newMode) / (SLIDER_PULLEY_TEETH * 2.0); //Stepper motor has 200 steps per 360 degrees, the timing pully has 36 teeth and the belt has a pitch of 2mm

    stepper_pan.setMaxSpeed(panDegreesToSteps(pan_max_speed));
    stepper_tilt.setMaxSpeed(tiltDegreesToSteps(tilt_max_speed));
    stepper_slider.setMaxSpeed(sliderMillimetresToSteps(slider_max_speed));
    step_mode = newMode;
    logger.log("Set to ", step_mode, " step mode.\n");
    clearKeyframes();
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

void panDegrees(float angle){
    logger.log("Set panDegrees to ",angle);
    target_position[0] = panDegreesToSteps(angle);
    if(acceleration_enable_state == 0){
        multi_stepper.moveTo(target_position);
    }
    else{
        stepper_pan.setCurrentPosition(stepper_pan.currentPosition());
        stepper_pan.runToNewPosition(panDegreesToSteps(angle));
    } 
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

void tiltDegrees(float angle){
    logger.log("Set tiltDegrees to ", angle);
    target_position[1] = tiltDegreesToSteps(angle);
    if(acceleration_enable_state == 0){
        multi_stepper.moveTo(target_position);
    }
    else{
        stepper_tilt.setCurrentPosition(stepper_tilt.currentPosition());
        stepper_tilt.runToNewPosition(tiltDegreesToSteps(angle));
    }
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

float panDegreesToSteps(float angle){
    return pan_steps_per_degree * angle;
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

float tiltDegreesToSteps(float angle){
    return tilt_steps_per_degree * angle;
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

long sliderMillimetresToSteps(float mm){
    return round(mm * slider_steps_per_millimetre);
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

float sliderStepsToMillimetres(long steps){
    return (float)steps / slider_steps_per_millimetre;
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

void sliderMoveTo(float mm){
    logger.log("Move slider to ", mm);
    target_position[2] = sliderMillimetresToSteps(mm);
    if(acceleration_enable_state == 0){ 
        multi_stepper.moveTo(target_position);
    }
    else{
        stepper_slider.setCurrentPosition(stepper_slider.currentPosition());
        stepper_slider.runToNewPosition(sliderMillimetresToSteps(mm));
    }    
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

void printKeyframeElements(void){
    logger.log("Keyframe index: ", current_keyframe_index, "\n");
    for(int row = 0; row < keyframe_elements; row++){
        logger.log("", row, "\t|");
        logger.log(" Pan: ", panStepsToDegrees(keyframe_array[row].panStepCount), 3, "º\t");
        logger.log("Tilt: ", tiltStepsToDegrees(keyframe_array[row].tiltStepCount), 3, "º\t");
        logger.log("Slider: ", sliderStepsToMillimetres(keyframe_array[row].sliderStepCount), 3, "mm\t");
        logger.log("Pan Speed: ", panStepsToDegrees(keyframe_array[row].panSpeed), 3, " º/s\t");
        logger.log("Tilt Speed: ", tiltStepsToDegrees(keyframe_array[row].tiltSpeed), 3, " º/s\t");  
        logger.log("Slider Speed: ", sliderStepsToMillimetres(keyframe_array[row].sliderSpeed), 3, " mm/s\t");      
        logger.log("Delay: ", keyframe_array[row].msDelay, "ms |\n");  
    }
    logger.log("\n");
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

void debugReport(void){
    logger.log("Status\n");
    logger.log("Status\nEnable state: ", enable_state);
//    logger.log(F("Step Mode: "), step_mode);
    //logger.log("Pan angle: ", panStepsToDegrees(stepper_pan.currentPosition()), 3, "º\n");
    //logger.log("Tilt angle: ", tiltStepsToDegrees(stepper_tilt.currentPosition()), 3, "º\n"); 
    //logger.log("Slider position: ", sliderStepsToMillimetres(stepper_slider.currentPosition()), 3, "mm\n");  
    //logger.log("Pan max steps/s: ", stepper_pan.maxSpeed());
    //logger.log("Tilt max steps/s: ", stepper_tilt.maxSpeed());
    //logger.log("Slider max steps/s: ", stepper_slider.maxSpeed());
    //logger.log("Pan max speed: ", panStepsToDegrees(stepper_pan.maxSpeed()), 3, "º/s\n");
    //logger.log("Tilt max speed: ", tiltStepsToDegrees(stepper_tilt.maxSpeed()), 3, "º/s\n");
    //logger.log("Slider max speed: ", sliderStepsToMillimetres(stepper_slider.maxSpeed()), 3, "mm/s\n");        
    //logger.log("Battery: ", getBatteryPercentage(), 3, "%\n");
    logger.log("Battery: ", getBatteryVoltage(), 3, "V\n");
//    logger.log("Homing mode: ", homing_mode);    
    logger.log("Angle between pics: ", degrees_per_picture, 3, "º\n");
    logger.log("Panoramiclapse delay between pics: ", delay_ms_between_pictures, "ms\n");   
    logger.log(VERSION_NUMBER);
    //printEEPROM();
    printKeyframeElements();
//    logger.log("\n");
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

void setTargetPositions(float panDeg, float tiltDeg, float sliderMillimetre){
    target_position[0] = panDegreesToSteps(panDeg);
    target_position[1] = tiltDegreesToSteps(tiltDeg);
    target_position[2] = sliderMillimetresToSteps(sliderMillimetre);
    multi_stepper.moveTo(target_position); 
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

bool findHome(void){
    bool panHomeFlag = false;
    bool tiltHomeFlag = false;
    bool sliderHomeFlag = false;
    int panHomingDir = -1;
    int tiltHomingDir = -1; 
      
    target_position[0] = stepper_pan.currentPosition();
    target_position[1] = stepper_tilt.currentPosition();
    target_position[2] = stepper_slider.currentPosition();
    
    if(homing_mode == 0){ //No homing
        return false;  
    }
    
    if(homing_mode == 1 || homing_mode == 3){ //Home the slider
        while(digitalRead(PIN_SLIDER_HALL) == 0){ //Move off the hall
            target_position[2] = target_position[2] + sliderMillimetresToSteps(0.1); 
            multi_stepper.moveTo(target_position); 
            multi_stepper.runSpeedToPosition();        
        }
        setTargetPositions(panStepsToDegrees(target_position[0]), tiltStepsToDegrees(target_position[1]), -1000);//1000 is about the length of the slider
        while(multi_stepper.run()){
            if(digitalRead(PIN_SLIDER_HALL) == 0){
                stepper_slider.setCurrentPosition(0);//set step count to 0
                setTargetPositions(panStepsToDegrees(target_position[0]), tiltStepsToDegrees(target_position[1]), 0);
                sliderHomeFlag = true;
            }
        }
    }

    long sliderPos = sliderStepsToMillimetres(stepper_slider.currentPosition());
    
    if(homing_mode == 2 || homing_mode == 3){ //Home pan and tilt
        while(digitalRead(PIN_PAN_HALL) == 0 || digitalRead(PIN_TILT_HALL) == 0){//If already on a Hall sensor move off
            target_position[0] = target_position[0] + panDegreesToSteps(!digitalRead(PIN_PAN_HALL));//increment by 1 degree
            target_position[1] = target_position[1] + tiltDegreesToSteps(!digitalRead(PIN_TILT_HALL));//increment by 1 degree
            if(target_position[0] > panDegreesToSteps(360) && target_position[1] > tiltDegreesToSteps(360)){//If both axis have done more than a full rotation there must be an issue...
                return false;
            }
            multi_stepper.moveTo(target_position); 
            multi_stepper.runSpeedToPosition();
        }
        stepper_pan.setCurrentPosition(0);//set step count to 0
        stepper_tilt.setCurrentPosition(0);//set step count to 0
    
        setTargetPositions(-45, -45, sliderPos);
        while(multi_stepper.run()){
            if(digitalRead(PIN_PAN_HALL) == 0){
                stepper_pan.setCurrentPosition(0);//set step count to 0
                setTargetPositions(0, -45 * !tiltHomeFlag, sliderPos);
                panHomeFlag = true;
                panHomingDir = 1;
            }
            if(digitalRead(PIN_TILT_HALL) == 0){
                stepper_tilt.setCurrentPosition(0);
                setTargetPositions(-45 * !panHomeFlag, 0, sliderPos);
                tiltHomeFlag = true;
                tiltHomingDir = 1;
            }
        }     
        
        setTargetPositions(360 * !panHomeFlag, 360 * !tiltHomeFlag, sliderPos);//full rotation on both axis so it must pass the home position
        while(multi_stepper.run()){
            if(digitalRead(PIN_PAN_HALL) == 0){
                stepper_pan.setCurrentPosition(0);//set step count to 0
                setTargetPositions(0, 360 * !tiltHomeFlag, sliderPos);
                panHomeFlag = true;
            }
            if(digitalRead(PIN_TILT_HALL)  == 0){
                stepper_tilt.setCurrentPosition(0);
                setTargetPositions(360 * !panHomeFlag, 0, sliderPos);
                tiltHomeFlag = true;
            }
        } 
    }
    
    if(panHomeFlag && tiltHomeFlag && (homing_mode == 2 || homing_mode == 3)){
        setTargetPositions(hall_pan_offset_degrees * panHomingDir, hall_tilt_offset_degrees * tiltHomingDir, sliderPos);
        multi_stepper.runSpeedToPosition();
        stepper_pan.setCurrentPosition(0);//set step count to 0
        stepper_tilt.setCurrentPosition(0);//set step count to 0
        setTargetPositions(0, 0, sliderPos);
        if(homing_mode == 3 && sliderHomeFlag == false){
            return false;
        }
        else{
            return true;
        }
    }
    else if(sliderHomeFlag){
        return true;
    }
    else{
        return false;
    }
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

float getBatteryVoltage(void){ //TODO: Calibrate the values for your battery
    return HelpFunctions::mapNumber(analogRead(PIN_INPUT_VOLTAGE), 0, 1007, 0, 12.6);//1007 = 12.6V
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

float getBatteryPercentage(void){ //TODO: Calibrate the values for your battery
    return boundFloat(HelpFunctions::mapNumber(getBatteryVoltage(), 9, 12.6, 0, 100), 0, 100); //780 = 9V = 0%, 1023 = 12.6V = 100%
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

float panStepsToDegrees(long steps){
    return steps / pan_steps_per_degree;
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

float panStepsToDegrees(float steps){
    return steps / pan_steps_per_degree;
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

float tiltStepsToDegrees(long steps){
    return steps / tilt_steps_per_degree;
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

float tiltStepsToDegrees(float steps){
    return steps / tilt_steps_per_degree;
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

int addPosition(void){
    if(keyframe_elements >= 0 && keyframe_elements < KEYFRAME_ARRAY_LENGTH){
        keyframe_array[keyframe_elements].panStepCount = stepper_pan.currentPosition();
        keyframe_array[keyframe_elements].tiltStepCount = stepper_tilt.currentPosition();    
        keyframe_array[keyframe_elements].sliderStepCount = stepper_slider.currentPosition();    
        keyframe_array[keyframe_elements].panSpeed = stepper_pan.maxSpeed();
        keyframe_array[keyframe_elements].tiltSpeed = stepper_tilt.maxSpeed();    
        keyframe_array[keyframe_elements].sliderSpeed = stepper_slider.maxSpeed();      
        keyframe_array[keyframe_elements].msDelay = 0;      
        current_keyframe_index = keyframe_elements;
        keyframe_elements++;//increment the index
        logger.log("Added at index: ", current_keyframe_index);
        return 0;
    }
    else{
        logger.log("Max number of keyframes reached\n");
    }
    return -1;
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

void clearKeyframes(void){
    keyframe_elements = 0;
    current_keyframe_index = -1;
    logger.log("Keyframes cleared\n");
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

void moveToIndex(int index){
    if(index < keyframe_elements && index >= 0){
        target_position[0] = keyframe_array[index].panStepCount;
        target_position[1] = keyframe_array[index].tiltStepCount;
        target_position[2] = keyframe_array[index].sliderStepCount;
        stepper_pan.setMaxSpeed(keyframe_array[index].panSpeed);
        stepper_tilt.setMaxSpeed(keyframe_array[index].tiltSpeed);
        stepper_slider.setMaxSpeed(keyframe_array[index].sliderSpeed);

        if(acceleration_enable_state == 0){ //If accelerations are not enabled just move directly to the target position. 
            multi_stepper.moveTo(target_position); //Sets new target positions
            multi_stepper.runSpeedToPosition(); //Moves and blocks until complete
            delay(keyframe_array[index].msDelay);
            current_keyframe_index = index;
            return;
        }
        
        float panInitialSpeed = stepper_pan.speed();
        float tiltInitialSpeed = stepper_tilt.speed();
        float sliderInitialSpeed = stepper_slider.speed();

        if(index >= 1){
            if(keyframe_array[index - 1].msDelay != 0){
                panInitialSpeed = 0;
                tiltInitialSpeed = 0;
                sliderInitialSpeed = 0;
            }
        }
        
        multi_stepper.moveTo(target_position); //Sets new target positions //sets speeds

        float panDeltaSpeed = stepper_pan.speed() - panInitialSpeed;
        float tiltDeltaSpeed = stepper_tilt.speed() - tiltInitialSpeed;
        float sliderDeltaSpeed = stepper_slider.speed() - sliderInitialSpeed;
        
        float panAccel = stepper_pan.speed() / (pan_accel_increment_us * 0.0001); //Equation is arbitrary and was deterined through empirical testing. The acceleration value does NOT correspond to mm/s/s
        float tiltAccel = stepper_tilt.speed() / (tilt_accel_increment_us * 0.0001);
        float sliderAccel = stepper_slider.speed() / (slider_accel_increment_us * 0.0001);   

        long panDist = 0;
        long tiltDist = 0;
        long sliderDist = 0;
        
        if(panAccel != 0){
            panDist = pow(stepper_pan.speed(), 2) / (5 * panAccel); //Equation is arbitrary and was deterined through empirical testing.
        }
        if(tiltAccel != 0){
            tiltDist = pow(stepper_tilt.speed(), 2) / (5 * tiltAccel); //Equation is arbitrary and was deterined through empirical testing.
        }
        if(sliderAccel != 0){
            sliderDist = pow(stepper_slider.speed(), 2) / (5 * sliderAccel); //Equation is arbitrary and was deterined through empirical testing.
        }

        if(index + 1 < keyframe_elements){//makes sure there is a valid next keyframe
            if(keyframe_array[index].msDelay == 0){
                long panStepDiff = keyframe_array[index + 1].panStepCount - keyframe_array[index].panStepCount; //Change in position from current target position to the next.
                long tiltStepDiff = keyframe_array[index + 1].tiltStepCount - keyframe_array[index].tiltStepCount;
                long sliderStepDiff = keyframe_array[index + 1].sliderStepCount - keyframe_array[index].sliderStepCount;
                if((panStepDiff == 0 && stepper_pan.speed() != 0) || (panStepDiff > 0 && stepper_pan.speed() < 0) || (panStepDiff < 0 && stepper_pan.speed() > 0)){ //if stopping or changing direction
                    target_position[0] = keyframe_array[index].panStepCount - panDist; //Set the target position slightly before the actual target to allow for the distance traveled while decelerating.
                }
                if((tiltStepDiff == 0 && stepper_tilt.speed() != 0) || (tiltStepDiff > 0 && stepper_tilt.speed() < 0) || (tiltStepDiff < 0 && stepper_tilt.speed() > 0)){ //if stopping or changing direction
                    target_position[1] = keyframe_array[index].tiltStepCount - tiltDist;
                }
                if((sliderStepDiff == 0 && stepper_slider.speed() != 0) || (sliderStepDiff > 0 && stepper_slider.speed() < 0) || (sliderStepDiff < 0 && stepper_slider.speed() > 0)){ //if stopping or changing direction
                    target_position[2] = keyframe_array[index].sliderStepCount - sliderDist;//If changing dir
                }
            }
        }

        if(index > 0){
            long panStepDiffPrev = keyframe_array[index].panStepCount - keyframe_array[index - 1].panStepCount; //Change in position from the privious target to the current target position.
            long tiltStepDiffPrev = keyframe_array[index].tiltStepCount - keyframe_array[index - 1].tiltStepCount;
            long sliderStepDiffPrev = keyframe_array[index].sliderStepCount - keyframe_array[index - 1].sliderStepCount;
            if(panStepDiffPrev == 0 && panDeltaSpeed == 0){ //Movement stopping
                panDeltaSpeed = -(2 * stepper_pan.speed()); //Making it negative ramps the speed down in the acceleration portion of the movement. The multiplication factor is arbitrary and was deterined through empirical testing.
            }
            if(tiltStepDiffPrev == 0 && tiltDeltaSpeed == 0){ //Movement stopping
                tiltDeltaSpeed = -(2 * stepper_tilt.speed());
            }
            if(sliderStepDiffPrev == 0 && sliderDeltaSpeed == 0){ //Movement stopping
                sliderDeltaSpeed = -(2 * stepper_slider.speed());
            }
        }
        
        multi_stepper.moveTo(target_position); //Sets new target positions and calculates new speeds.
        
        if(stepper_pan.currentPosition() != target_position[0] || stepper_tilt.currentPosition() != target_position[1] || stepper_slider.currentPosition() != target_position[2]){ //Prevents issues caused when the motor target positions and speeds not being updated becuase they have not changed.
            //Impliments the acceleration/deceleration. This implimentation feels pretty bad and should probably be updated but it works well enough so I'm not going to...
            float panInc = 0;
            float tiltInc = 0;
            float sliderInc = 0;
            unsigned long pan_last_us = 0;
            unsigned long tilt_last_us = 0;
            unsigned long slider_last_us = 0;
            
            while(((panInc < 1) || (tiltInc < 1) || (sliderInc < 1)) && multi_stepper.run()){
                unsigned long usTime = micros();
                
                if(usTime - pan_accel_increment_us >= pan_last_us){
                    panInc = (panInc < 1) ? (panInc + 0.01) : 1;
                    pan_last_us = micros();
                    stepper_pan.setSpeed(panInitialSpeed + (panDeltaSpeed * panInc));
                }
                
                if(usTime - tilt_accel_increment_us >= tilt_last_us){
                    tiltInc = (tiltInc < 1) ? (tiltInc + 0.01) : 1;
                    tilt_last_us = micros();
                    stepper_tilt.setSpeed(tiltInitialSpeed + (tiltDeltaSpeed * tiltInc));
                }
                
                if(usTime - slider_accel_increment_us >= slider_last_us){
                    sliderInc = (sliderInc < 1) ? (sliderInc + 0.01) : 1;
                    slider_last_us = micros();
                    stepper_slider.setSpeed(sliderInitialSpeed + (sliderDeltaSpeed * sliderInc));
                }
            }

            multi_stepper.moveTo(target_position); //Sets all speeds to reach the target
            multi_stepper.runSpeedToPosition(); //Moves and blocks until complete
        }
        delay(keyframe_array[index].msDelay);
        current_keyframe_index = index;
    }
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

void executeMoves(int repeat){
    stepper_pan.setSpeed(0);
    stepper_tilt.setSpeed(0);
    stepper_slider.setSpeed(0);
    for(int i = 0; i < repeat; i++){
        for(int row = 0; row < keyframe_elements; row++){
            moveToIndex(row);
        }
//        if(getBatteryVoltage() < 9.5){//9.5V is used as the cut off to allow for inaccuracies and be on the safe side.
//            delay(200);
//            if(getBatteryVoltage() < 9.5){//Check voltage is still low and the first wasn't a miscellaneous reading
//                logger.log(F("Battery low"));
//                while(1){}//loop and do nothing
//            }
//        }
    }
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

void gotoFirstKeyframe(void){
    moveToIndex(0);
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

void gotoLastKeyframe(void){
    moveToIndex(keyframe_elements - 1);
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

void editKeyframe(void){
    keyframe_array[current_keyframe_index].panStepCount = stepper_pan.currentPosition();
    keyframe_array[current_keyframe_index].tiltStepCount = stepper_tilt.currentPosition(); 
    keyframe_array[current_keyframe_index].sliderStepCount = stepper_slider.currentPosition(); 
    keyframe_array[current_keyframe_index].panSpeed = stepper_pan.maxSpeed();
    keyframe_array[current_keyframe_index].tiltSpeed = stepper_tilt.maxSpeed();
    keyframe_array[current_keyframe_index].sliderSpeed = stepper_slider.maxSpeed();
    
    logger.log("Edited index: ", current_keyframe_index);
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

void editDelay(unsigned int ms){
    keyframe_array[current_keyframe_index].msDelay = ms;
    logger.log(ms, "");
    logger.log("ms delay added at index: ", current_keyframe_index);
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

void addDelay(unsigned int ms){
    addPosition();
    keyframe_array[current_keyframe_index].msDelay = ms;
    logger.log(ms, "");
    logger.log("ms delay added at index: ", current_keyframe_index);
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

void invertPanDirection(bool invert){
    logger.log("Pan inversion: ", invert);
    invert_pan = invert;
    stepper_pan.setPinsInverted(invert, false, false);
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

void invertTiltDirection(bool invert){
    logger.log("Tilt inversion: ", invert);
    invert_tilt = invert;
    stepper_tilt.setPinsInverted(invert, false, false);
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

void invertSliderDirection(bool invert){
    logger.log("Slider inversion: ", invert);
    invert_slider = invert;
    stepper_slider.setPinsInverted(invert, false, false);
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

/*
void saveEEPROM(void){
    EEPROM.put(EEPROM_ADDRESS_HOMING_MODE, homing_mode);
    EEPROM.put(EEPROM_ADDRESS_MODE, step_mode);
    EEPROM.put(EEPROM_ADDRESS_PAN_MAX_SPEED, pan_max_speed);
    EEPROM.put(EEPROM_ADDRESS_TILT_MAX_SPEED, tilt_max_speed);
    EEPROM.put(EEPROM_ADDRESS_SLIDER_MAX_SPEED, slider_max_speed);
    EEPROM.put(EEPROM_ADDRESS_HALL_PAN_OFFSET, hall_pan_offset_degrees);
    EEPROM.put(EEPROM_ADDRESS_HALL_TILT_OFFSET, hall_tilt_offset_degrees);
    EEPROM.put(EEPROM_ADDRESS_INVERT_PAN, invert_pan);
    EEPROM.put(EEPROM_ADDRESS_INVERT_TILT, invert_tilt);    
    EEPROM.put(EEPROM_ADDRESS_INVERT_SLIDER, invert_slider);      
    EEPROM.put(EEPROM_ADDRESS_DEGREES_PER_PICTURE, degrees_per_picture);
    EEPROM.put(EEPROM_ADDRESS_PANORAMICLAPSE_DELAY, delay_ms_between_pictures);
    EEPROM.put(EEPROM_ADDRESS_ACCELERATION_ENABLE, acceleration_enable_state);
    EEPROM.put(EEPROM_ADDRESS_PAN_ACCEL_INCREMENT_DELAY, pan_accel_increment_us);
    EEPROM.put(EEPROM_ADDRESS_TILT_ACCEL_INCREMENT_DELAY, tilt_accel_increment_us);
    EEPROM.put(EEPROM_ADDRESS_SLIDER_ACCEL_INCREMENT_DELAY, slider_accel_increment_us);
}
*/

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

/*
void printEEPROM(void){
    int itemp;
    float ftemp;
    long ltemp;
//    logger.log(F("EEPROM:\n"));
    EEPROM.get(EEPROM_ADDRESS_MODE, itemp);
    logger.log("EEPROM:\nStep mode: ", itemp, "\n");
    EEPROM.get(EEPROM_ADDRESS_PAN_MAX_SPEED, ftemp);
    logger.log("Pan max: ", ftemp, 3, "º/s\n");
    EEPROM.get(EEPROM_ADDRESS_TILT_MAX_SPEED, ftemp);
    logger.log("Tilt max: ", ftemp, 3, "º/s\n");
    EEPROM.get(EEPROM_ADDRESS_SLIDER_MAX_SPEED, ftemp);
    logger.log("Slider max: ", ftemp, 3, "mm/s\n"); 
    EEPROM.get(EEPROM_ADDRESS_HALL_PAN_OFFSET, ftemp);
    logger.log("Pan offset: ", ftemp, 3, "º\n");
    EEPROM.get(EEPROM_ADDRESS_HALL_TILT_OFFSET, ftemp);
    logger.log("Tilt offset: ", ftemp, 3, "º\n");
    EEPROM.get(EEPROM_ADDRESS_DEGREES_PER_PICTURE, ftemp);
    logger.log("Angle between pics: ", ftemp, 3, " º\n");
    EEPROM.get(EEPROM_ADDRESS_PANORAMICLAPSE_DELAY, ltemp);
    logger.log("Delay between pics: ", ltemp, "ms\n");   
    logger.log("Pan invert: ", EEPROM.read(EEPROM_ADDRESS_INVERT_PAN));
    logger.log("Tilt invert: ", EEPROM.read(EEPROM_ADDRESS_INVERT_TILT));
    logger.log("Slider invert: ", EEPROM.read(EEPROM_ADDRESS_INVERT_SLIDER)); 
    logger.log("Homing mode: ", EEPROM.read(EEPROM_ADDRESS_HOMING_MODE));
    logger.log("Accel enable: ", EEPROM.read(EEPROM_ADDRESS_ACCELERATION_ENABLE));
    EEPROM.get(EEPROM_ADDRESS_PAN_ACCEL_INCREMENT_DELAY, itemp);
    logger.log("Pan accel delay: ", itemp, "us\n");
    EEPROM.get(EEPROM_ADDRESS_TILT_ACCEL_INCREMENT_DELAY, itemp);
    logger.log("Tilt accel delay: ", itemp, "us\n");
    EEPROM.get(EEPROM_ADDRESS_SLIDER_ACCEL_INCREMENT_DELAY, itemp);
    logger.log("Slider accel delay: ", itemp, "us\n");
}
*/

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

/*
void setEEPROMVariables(void){
    EEPROM.get(EEPROM_ADDRESS_MODE, step_mode);
    EEPROM.get(EEPROM_ADDRESS_PAN_MAX_SPEED, pan_max_speed);
    EEPROM.get(EEPROM_ADDRESS_TILT_MAX_SPEED, tilt_max_speed);
    EEPROM.get(EEPROM_ADDRESS_SLIDER_MAX_SPEED, slider_max_speed);
    EEPROM.get(EEPROM_ADDRESS_HALL_PAN_OFFSET, hall_pan_offset_degrees);
    EEPROM.get(EEPROM_ADDRESS_HALL_TILT_OFFSET, hall_tilt_offset_degrees);
    EEPROM.get(EEPROM_ADDRESS_DEGREES_PER_PICTURE, degrees_per_picture);
    EEPROM.get(EEPROM_ADDRESS_PANORAMICLAPSE_DELAY, delay_ms_between_pictures);
    EEPROM.get(EEPROM_ADDRESS_PAN_ACCEL_INCREMENT_DELAY, pan_accel_increment_us);
    EEPROM.get(EEPROM_ADDRESS_TILT_ACCEL_INCREMENT_DELAY, tilt_accel_increment_us);
    EEPROM.get(EEPROM_ADDRESS_SLIDER_ACCEL_INCREMENT_DELAY, slider_accel_increment_us);        
    invert_pan = EEPROM.read(EEPROM_ADDRESS_INVERT_PAN);
    invert_tilt = EEPROM.read(EEPROM_ADDRESS_INVERT_TILT);
    invert_slider = EEPROM.read(EEPROM_ADDRESS_INVERT_SLIDER);
    homing_mode = EEPROM.read(EEPROM_ADDRESS_HOMING_MODE);
    acceleration_enable_state = EEPROM.read(EEPROM_ADDRESS_ACCELERATION_ENABLE);
}
*/

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

void setHoming(byte homingType){
    if(homingType >= 0 && homingType <= 4){
        homing_mode = homingType;
        logger.log("Homing set to mode ", homingType, "\n");
    }
    else{
        logger.log("Invalid mode\n");
    }
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

void triggerCameraShutter(void){
    digitalWrite(PIN_SHUTTER_TRIGGER, HIGH);
    delay(SHUTTER_DELAY);
    digitalWrite(PIN_SHUTTER_TRIGGER, LOW);
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

void panoramiclapseInterpolation(float panStartAngle, float tiltStartAngle, float sliderStartPos, float panStopAngle, float tiltStopAngle, float sliderStopPos, float degPerPic, unsigned long msDelay){
    if(degPerPic == 0) return;
    if(msDelay > SHUTTER_DELAY){
        msDelay = msDelay - SHUTTER_DELAY;
    }
    float panAngle = panStopAngle - panStartAngle;
    float tiltAngle = tiltStopAngle - tiltStartAngle;
    float sliderDistance = sliderStopPos - sliderStartPos;
    float largestAngle = (abs(panAngle) > abs(tiltAngle)) ? panAngle : tiltAngle;
    unsigned int numberOfIncrements = abs(largestAngle) / degPerPic;
    if(numberOfIncrements == 0) return;
    float panInc = panAngle / numberOfIncrements;
    float tiltInc = tiltAngle / numberOfIncrements;
    float sliderInc = sliderDistance / numberOfIncrements;
    
    for(unsigned int i = 0; i <= numberOfIncrements; i += 1){
        setTargetPositions(panStartAngle + (panInc * i), tiltStartAngle + (tiltInc * i), sliderStartPos + (sliderInc * i));
        multi_stepper.runSpeedToPosition();//blocking move to the next position
        delay(msDelay / 2);
        triggerCameraShutter();//capture the picture
        delay(msDelay / 2);
    }
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

void panoramiclapse(float degPerPic, unsigned long msDelay, int repeat){   
    if(keyframe_elements < 2){ 
        logger.log("Not enough keyframes\n");
        return; //check there are posions to move to
    }
    for(unsigned int i = 0; i < repeat; i += 1){
        for(int index = 0; index < keyframe_elements - 1; index++){
            panoramiclapseInterpolation(panStepsToDegrees(keyframe_array[index].panStepCount), tiltStepsToDegrees(keyframe_array[index].tiltStepCount), sliderStepsToMillimetres(keyframe_array[index].sliderStepCount),
            panStepsToDegrees(keyframe_array[index + 1].panStepCount), tiltStepsToDegrees(keyframe_array[index + 1].tiltStepCount), sliderStepsToMillimetres(keyframe_array[index + 1].sliderStepCount), degPerPic, msDelay);
        }
    }
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

void timelapse(unsigned int numberOfPictures, unsigned long msDelay){
    if(msDelay > SHUTTER_DELAY){
        msDelay = msDelay - SHUTTER_DELAY;
    }
    
    if(numberOfPictures > 1){
        numberOfPictures = numberOfPictures - 1;
    }
    
    unsigned long halfDelay = msDelay / 2;
   
    float panAngle = 0;
    float tiltAngle = 0;
    float sliderTravel = 0;
    
    if(keyframe_elements >= 2){ 
        panAngle = panStepsToDegrees(keyframe_array[1].panStepCount) - panStepsToDegrees(keyframe_array[0].panStepCount);
        tiltAngle = tiltStepsToDegrees(keyframe_array[1].tiltStepCount) - tiltStepsToDegrees(keyframe_array[0].tiltStepCount);
        sliderTravel = sliderStepsToMillimetres(keyframe_array[1].sliderStepCount) - sliderStepsToMillimetres(keyframe_array[0].sliderStepCount);
    }
    
    float sliderInc = sliderTravel / numberOfPictures;
    float panInc = panAngle / numberOfPictures;
    float tiltInc = tiltAngle / numberOfPictures;

    setTargetPositions(panStepsToDegrees(keyframe_array[0].panStepCount), tiltStepsToDegrees(keyframe_array[0].tiltStepCount), sliderStepsToMillimetres(keyframe_array[0].sliderStepCount));
    multi_stepper.runSpeedToPosition();//blocking move to the next position
    
    for(unsigned int i = 0; i <= numberOfPictures; i += 1){
        setTargetPositions(panStepsToDegrees(keyframe_array[0].panStepCount) + (panInc * i), tiltStepsToDegrees(keyframe_array[0].tiltStepCount) + (tiltInc * i), sliderStepsToMillimetres(keyframe_array[0].sliderStepCount) + (sliderInc * i));
        multi_stepper.runSpeedToPosition();//blocking move to the next position
        delay(halfDelay);
        triggerCameraShutter();//capture the picture
        delay(halfDelay);
    }
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/
//From the first two keyframes the intercept of where the camera is directed is calculated.
//The first kayframe's x pan and tilt positions are used to calculate a 3D vector. The second keyframe's x and pan position are used to calculate a vertical plane. (It wuld be almost impossible for 2 3D vectors to intercept due to floating point precision issues.)
//The intercept of the vectorand plane are then calculated to give the X, Y, Z coordinates of the point the camera was pointed at in both keyframes. (The second keyframe will ignore the tilt value and calculate it based on the first keyframes vector.)
bool calculateTargetCoordinate(void){ 
    float m1, c1, m2, c2 = 0;
    
    LinePoints line0;
    line0.x0 = sliderStepsToMillimetres(keyframe_array[0].sliderStepCount);
    line0.y0 = 0;
    line0.x1 = line0.x0 + cos(HelpFunctions::degToRads(panStepsToDegrees(keyframe_array[0].panStepCount)));
    line0.y1 = sin(HelpFunctions::degToRads(panStepsToDegrees(keyframe_array[0].panStepCount)));
    
    LinePoints line1;
    line1.x0 = sliderStepsToMillimetres(keyframe_array[1].sliderStepCount);
    line1.y0 = 0;
    line1.x1 = line1.x0 + cos(HelpFunctions::degToRads(panStepsToDegrees(keyframe_array[1].panStepCount)));
    line1.y1 = sin(HelpFunctions::degToRads(panStepsToDegrees(keyframe_array[1].panStepCount)));
    
    if((line0.x1 - line0.x0) != 0){
        m1 = (line0.y1 - line0.y0) / (line0.x1 - line0.x0);
        c1 = line0.y1 - m1 * line0.x1;
    }

    if((line1.x1 - line1.x0) != 0){
        m2 = (line1.y1 - line1.y0) / (line1.x1 - line1.x0);
        c2 = line1.y1 - m2 * line1.x1;
    }

    if((line0.x1 - line0.x0) == 0){
        intercept.x = line0.x0;
        intercept.y = m2 * intercept.x + c2;    
    }
    else if((line1.x1 - line1.x0) == 0){
        intercept.x = line1.x0;
        intercept.y = m1 * intercept.x + c1;
    }
    else{
        if(m1 == m2){ //If the angle of the slope of both lines are the same they are parallel and cannot intercept.
            logger.log("Positions do not intersect.");
            return false;
        }
        intercept.x = (c2 - c1) / (m1 - m2);
        intercept.y = m1 * intercept.x + c1;
    }
    intercept.z = tan(HelpFunctions::degToRads(tiltStepsToDegrees(keyframe_array[0].tiltStepCount))) * sqrt(pow(intercept.x - sliderStepsToMillimetres(keyframe_array[0].sliderStepCount), 2) + pow(intercept.y, 2));
    if(((panStepsToDegrees(keyframe_array[0].panStepCount) > 0 && panStepsToDegrees(keyframe_array[1].panStepCount) > 0) && intercept.y < 0)
    || ((panStepsToDegrees(keyframe_array[0].panStepCount) < 0 && panStepsToDegrees(keyframe_array[1].panStepCount) < 0) && intercept.y > 0) || intercept.y == 0){ //Checks that the intercept point is in the direction the camera was pointing and not on the opposite side behind the camera.
        logger.log("Invalid intercept.\n");
        return false;
    }
    return true;
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

void interpolateTargetPoint(FloatCoordinate targetPoint, int repeat){ //The first two keyframes are interpolated between while keeping the camera pointing at previously calculated intercept point.
    if(keyframe_elements < 2){ 
        logger.log("Not enough keyframes recorded\n");
        return; //check there are posions to move to
    }

    float sliderStartPos = sliderStepsToMillimetres(keyframe_array[0].sliderStepCount); //slider start position
    float sliderEndPos = sliderStepsToMillimetres(keyframe_array[1].sliderStepCount);
    float panAngle = 0;
    float tiltAngle = 0;
    float x = targetPoint.x - sliderStepsToMillimetres(keyframe_array[0].sliderStepCount);
    float ySqared = pow(targetPoint.y, 2);    
    float sliderTravel = sliderStepsToMillimetres(keyframe_array[1].sliderStepCount) - sliderStepsToMillimetres(keyframe_array[0].sliderStepCount);
    int numberOfIncrements = abs(sliderTravel);
    float increment = sliderTravel / numberOfIncrements;//size of interpolation increments in mm
    
    for(int j = 0; (j < repeat || (repeat == 0 && j == 0)); j++){
        for(int i = 0; i <= numberOfIncrements; i++){
            x = targetPoint.x - (sliderStartPos + increment * i);
            panAngle = HelpFunctions::radsToDeg(atan2(targetPoint.y, x));
            tiltAngle = HelpFunctions::radsToDeg(atan2(targetPoint.z, sqrt(pow(x, 2) + ySqared)));
            setTargetPositions(panAngle, tiltAngle, sliderStartPos + increment * i);
            multi_stepper.runSpeedToPosition();//blocking move to the next position
        }
        x = targetPoint.x - sliderEndPos;
        panAngle = HelpFunctions::radsToDeg(atan2(targetPoint.y, x));
        tiltAngle = HelpFunctions::radsToDeg(atan2(targetPoint.z, sqrt(pow(x, 2) + ySqared)));
        setTargetPositions(panAngle, tiltAngle, sliderEndPos);
        multi_stepper.runSpeedToPosition();//blocking move to the next position

        for(int i = numberOfIncrements; (i >= 0 && repeat > 0); i--){
            x = targetPoint.x - (sliderStartPos + increment * i);
            panAngle = HelpFunctions::radsToDeg(atan2(targetPoint.y, x));
            tiltAngle = HelpFunctions::radsToDeg(atan2(targetPoint.z, sqrt(pow(x, 2) + ySqared)));
            setTargetPositions(panAngle, tiltAngle, sliderStartPos + increment * i);
            multi_stepper.runSpeedToPosition();//blocking move to the next position
        }
        if(repeat > 0){
            setTargetPositions(panAngle, tiltAngle, sliderStartPos);
            multi_stepper.runSpeedToPosition();//blocking move to the next position 
        }     
    }
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

void toggleAcceleration(void){
    if(acceleration_enable_state == 0){
        acceleration_enable_state = 1;
        logger.log("Accel enabled.\n");
    }
    else{
        acceleration_enable_state = 0;
        logger.log("Accel disabled.\n");
    }
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

void scaleKeyframeSpeed(float scaleFactor){
    if(scaleFactor <= 0){//Make sure a valid speed factor was entered
        logger.log("Invalid factor\n");
        return; 
    }
    
    for(int row = 0; row < keyframe_elements; row++){
        keyframe_array[row].panSpeed *= scaleFactor;
        keyframe_array[row].tiltSpeed *= scaleFactor;
        keyframe_array[row].sliderSpeed *= scaleFactor;
    }
    logger.log("Keyframe speed scaled by ", scaleFactor, 3, "\n");
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/


void processCommand(char instruction, int serialCommandValueInt, float serialCommandValueFloat) {
    if(instruction == '+'){//The Bluetooth module sends a message starting with "+CONNECTING" which should be discarded.
        delay(100); //wait to make sure all data in the serial message has arived 
        serialOutput.serialFlush();//Clear any excess data in the serial buffer
        return;
    }
    switch(instruction){        
        case INSTRUCTION_SCALE_SPEED:{
            scaleKeyframeSpeed(serialCommandValueFloat);
        }
        break;
        case INSTRUCTION_PAN_ACCEL_INCREMENT_DELAY:{
            pan_accel_increment_us = (serialCommandValueInt >= 0) ? serialCommandValueInt : 0;
            logger.log("Pan accel delay: ", pan_accel_increment_us, "us\n");
        }
        break;
        case INSTRUCTION_TILT_ACCEL_INCREMENT_DELAY:{
            tilt_accel_increment_us = (serialCommandValueInt >= 0) ? serialCommandValueInt : 0;
            logger.log("Tilt accel delay: ", tilt_accel_increment_us, "us\n");
        }
        break;
        case INSTRUCTION_SLIDER_ACCEL_INCREMENT_DELAY:{
            slider_accel_increment_us = (serialCommandValueInt >= 0) ? serialCommandValueInt : 0;
            logger.log("Slider accel delay: ", slider_accel_increment_us, "us\n");
        }
        break;
        case INSTRUCTION_ACCEL_ENABLE:{
            toggleAcceleration();
        }
        break;
        case INSTRUCTION_SLIDER_MILLIMETRES:{
            sliderMoveTo(serialCommandValueFloat);
        }
        break;
        case INSTRUCTION_DELAY_BETWEEN_PICTURES:{
            delay_ms_between_pictures = serialCommandValueFloat;
            logger.log("Delay between pics: ", delay_ms_between_pictures, "ms\n");
        }
        break;
        case INSTRUCTION_ANGLE_BETWEEN_PICTURES:{
            degrees_per_picture = serialCommandValueFloat;
            logger.log("Degs per pic: ", degrees_per_picture, 3, "º\n");
        }
        break;     
        case INSTRUCTION_PANORAMICLAPSE:{
            logger.log("Panorama\n");
            panoramiclapse(degrees_per_picture, delay_ms_between_pictures, 1);
            logger.log("Finished\n");
        }
        break;
        case INSTRUCTION_TIMELAPSE:{
            logger.log("Timelapse with ", serialCommandValueInt, " pics\n");
            logger.log("", delay_ms_between_pictures, "ms between pics\n");
            timelapse(serialCommandValueInt, delay_ms_between_pictures);
            logger.log("Finished\n");
        }
        break;
        case INSTRUCTION_TRIGGER_SHUTTER:{
            triggerCameraShutter();
        }
        break;
        case INSTRUCTION_AUTO_HOME:{
            logger.log("Homing\n");
            if(findHome()){
                logger.log("Complete\n");
            }
            else{
                stepper_pan.setCurrentPosition(0);
                stepper_tilt.setCurrentPosition(0);
                stepper_slider.setCurrentPosition(0);
                setTargetPositions(0, 0, 0);
                logger.log("Error homing\n");
            }
        }
        break;
        case INSTRUCTION_SET_HOMING:{
            setHoming(serialCommandValueInt);
        }
        break;
        case INSTRUCTION_SET_PAN_HALL_OFFSET:{
            hall_pan_offset_degrees = serialCommandValueFloat;
            logger.log("Pan offset: ", hall_pan_offset_degrees, 3, "º\n");
        }
        break;
        case INSTRUCTION_SET_TILT_HALL_OFFSET:{
            hall_tilt_offset_degrees = serialCommandValueFloat;
            logger.log("Tilt offset: ", hall_tilt_offset_degrees, 3, "º\n");
        }
        break;
         case INSTRUCTION_INVERT_SLIDER:{
            invertSliderDirection(serialCommandValueInt);
        }
        break;
        case INSTRUCTION_INVERT_TILT:{
            invertTiltDirection(serialCommandValueInt);
        }
        break;
        case INSTRUCTION_INVERT_PAN:{
            invertPanDirection(serialCommandValueInt);
        }
        break;
        case INSTRUCTION_SAVE_TO_EEPROM:{
            //saveEEPROM();
            logger.log("Saved to EEPROM\n");
        }
        break;
        case INSTRUCTION_ADD_POSITION:{
            addPosition();
        }
        break;
        case INSTRUCTION_STEP_FORWARD:{
            moveToIndex(current_keyframe_index + 1);
            logger.log("Index: ", current_keyframe_index, "\n");
        }
        break;
        case INSTRUCTION_STEP_BACKWARD:{
            moveToIndex(current_keyframe_index - 1);
            logger.log("Index: ", current_keyframe_index, "\n");
        }
        break;
        case INSTRUCTION_JUMP_TO_START:{
            gotoFirstKeyframe();
            logger.log("Index: ", current_keyframe_index, "\n");
        }
        break;
        case INSTRUCTION_JUMP_TO_END:{
            gotoLastKeyframe();
            logger.log("Index: ", current_keyframe_index, "\n");
        }
        break;
        case INSTRUCTION_EDIT_ARRAY:{
            editKeyframe();
        }
        break;
        case INSTRUCTION_ADD_DELAY:{
            addDelay(serialCommandValueInt);
        }
        break;
        case INSTRUCTION_EDIT_DELAY:{
            editDelay(serialCommandValueInt);
        }
        break;
        case INSTRUCTION_CLEAR_ARRAY:{
            clearKeyframes();
        }
        break;
        case INSTRUCTION_EXECUTE_MOVES:{
            executeMoves(serialCommandValueInt);
        }
        break;      
        case INSTRUCTION_DEBUG_STATUS:{
            debugReport();
        }
        break;
        case INSTRUCTION_PAN_DEGREES:{
            panDegrees(serialCommandValueFloat);
        }
        break;  
        case INSTRUCTION_TILT_DEGREES:{
            tiltDegrees(serialCommandValueFloat);
        }
        break; 
        case INSTRUCTION_ENABLE:{
            enableSteppers();
        }
        break; 
        case INSTRUCTION_STEP_MODE:{
            setStepMode(serialCommandValueInt);
        }
        break; 
        case INSTRUCTION_SET_PAN_SPEED:{
            logger.log("Max pan speed: ", serialCommandValueFloat, 1, "º/s.\n");
            pan_max_speed = serialCommandValueFloat;
            stepper_pan.setMaxSpeed(panDegreesToSteps(pan_max_speed));
        }
        break; 
        case INSTRUCTION_SET_TILT_SPEED:{
            logger.log("Max tilt speed: ", serialCommandValueFloat, 1, "º/s.\n");
            tilt_max_speed = serialCommandValueFloat;
            stepper_tilt.setMaxSpeed(tiltDegreesToSteps(tilt_max_speed));
        }
        break;
        case INSTRUCTION_SET_SLIDER_SPEED:{
            logger.log("Max slider speed: ", serialCommandValueFloat, 1, "mm/s.\n");
            slider_max_speed = serialCommandValueFloat;
            stepper_slider.setMaxSpeed(sliderMillimetresToSteps(slider_max_speed));
        }
        break;
        case INSTRUCTION_CALCULATE_TARGET_POINT:{            
            if(calculateTargetCoordinate()){
                logger.log("Target:\tx: ", intercept.x, 3, "\t");
                logger.log("y: ", intercept.y, 3, "\t");
                logger.log("z: ", intercept.z, 3, "mm\n");
            }
        }
        break;  
        case INSTRUCTION_ORIBIT_POINT:{            
            if(calculateTargetCoordinate()){
                interpolateTargetPoint(intercept, serialCommandValueInt);
            }
        }
        break;  
    }
}


// Here is your subArray function..
byte* subArray(byte* theArray,int itemSize,int startItem,int numItems) {
   int   trace;
   int   numBytes;
   byte* newArray;
   
   numBytes = itemSize * numItems;
   newArray = (byte*)malloc(numBytes);
   if (newArray) {
      trace = startItem * itemSize;
      for (int i=0;i<numBytes;i++) {
         newArray[i] = theArray[trace];
         trace++;
      }
   }
   return newArray;
}

void checkBle() {

    if(ble.isBleConnected()) {
          // Read from Smartphone
      if (ble.wasSerialCommandFromMobileDeviceReceived()) {
          String serialCommand = ble.getSerialCommandFromMobileDevice();

            char command = serialCommand[0];
            int serialCommandValueInt = atoi(serialCommand.substring(1, 19) .c_str());
            float serialCommandValueFloat = atof(serialCommand.substring(1, 19) .c_str());

            //int serialCommandValueInt = atoi((char*)subArray((byte*)bleSerialCommandFromMobileDevice,sizeof(char),1, 19));
            //float serialCommandValueFloat = atof((char*)subArray((byte*)bleSerialCommandFromMobileDevice,sizeof(char),1, 19));

            processCommand(command, serialCommandValueInt, serialCommandValueFloat);
        }
    }
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

void mainLoop(void){
    while(1){
        if(serialOutput.available()) {
            serialOutput.serialData();
        }
        checkBle();
        multi_stepper.run();
    }
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/
