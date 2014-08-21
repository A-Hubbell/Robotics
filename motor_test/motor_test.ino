/*
Title: Adafruit Motor Shield V2 Testing
Hardware: -4 DC Motors
          -Adafruit Motor Shield V2
          -Arduino Uno
*/

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"


//Create Adafruit motor shield object
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

//Create 4 DC motor objects   fl: front-left (M1), fr: front-right (M2), bl: back-left (M3), br: back-right (M4)
Adafruit_DCMotor *flMotor = AFMS.getMotor(1);
Adafruit_DCMotor *frMotor = AFMS.getMotor(2);
Adafruit_DCMotor *blMotor = AFMS.getMotor(3);
Adafruit_DCMotor *brMotor = AFMS.getMotor(4);



const int DEFAULT_SPEED = 50;   //negative values are treated as 255 minus the value (setSpeed is unsigned) 
const int DEFAULT_ACTIVE_TIME = 1000; //default "on time" of DC motors in microseconds,
                                      //could be re-written in terms of speed.

const int MOTOR_SELECTION = 1; //1-4 activate front-left, front-right, back-right, back-left, respectively
                               //5 activates all four motors

void setup()
{
  AFMS.begin();
  
  //Initialize the speed of all motors
  flMotor->setSpeed(DEFAULT_SPEED);
  frMotor->setSpeed(DEFAULT_SPEED);
  blMotor->setSpeed(DEFAULT_SPEED);
  brMotor->setSpeed(DEFAULT_SPEED);
  
  //Initialize the state of all motors
  flMotor->run(RELEASE);
  frMotor->run(RELEASE);
  blMotor->run(RELEASE);
  brMotor->run(RELEASE);
  
}

void loop()
{
  switch (MOTOR_SELECTION)
  {
    case 1:
            flMotor->run(FORWARD);
            delay(3000);
            flMotor->run(BACKWARD);
            delay(3000);
            flMotor->run(RELEASE);
            break;
    
    case 2: 
            frMotor->run(FORWARD);
            delay(3000);
            frMotor->run(BACKWARD);
            delay(3000);
            frMotor->run(RELEASE);
            break;
       
    case 3: 
            brMotor->run(FORWARD);
            delay(3000);
            brMotor->run(BACKWARD);
            delay(3000);
            brMotor->run(RELEASE);
            break;  
          
    case 4: 
            blMotor->run(FORWARD);
            delay(3000);
            blMotor->run(BACKWARD);
            delay(3000);
            blMotor->run(RELEASE);
            break;  
            
    case 5: 
            flMotor->run(FORWARD);
            frMotor->run(FORWARD);
            brMotor->run(FORWARD);
            blMotor->run(FORWARD);
            delay(3000);
            flMotor->run(BACKWARD);
            frMotor->run(BACKWARD);
            blMotor->run(BACKWARD);
            brMotor->run(BACKWARD);
            delay(3000);
            flMotor->run(RELEASE);
            frMotor->run(RELEASE);
            brMotor->run(RELEASE);
            blMotor->run(RELEASE);
            break;
  }
}
