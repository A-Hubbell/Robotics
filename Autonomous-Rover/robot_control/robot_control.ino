/*
Title: Autonomous Robot Control
Hardware: -4 wheel robot --> 4 DC Motors (controlled using Adafruit Arduino Motor Shield V2)
          -Ultrasonic range finder 
          -Servo motor attached to ultrasonic range finder for sweeping
          -Xbee series 2 wireless networking module

TODO: -Implement Adafruit motor shield servo code
      -Test rangefinder on rover
      -Calibrate "DEG_PER_SECOND" constant for a given rover speed
      -Integrate Xbee communications
*/

#include <Servo.h> //may not be necessary when using Adafruit motor shield
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"
#include "functions.h"



//Create Adafruit motor shield object
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

//Create 4 DC motor objects   fl: front-left (M1), fr: front-right (M2), bl: back-left (M3), br: back-right (M4)
Adafruit_DCMotor *flMotor = AFMS.getMotor(1);
Adafruit_DCMotor *frMotor = AFMS.getMotor(2);
Adafruit_DCMotor *blMotor = AFMS.getMotor(3);
Adafruit_DCMotor *brMotor = AFMS.getMotor(4);

//Create servo object for the rangefinder
Servo rangeServo;

//Ultrasonic rangefinder
const int RANGE_SIG_PIN = 4;
const int RANGE_TRIG_PIN = 5;
const int SERVO_CONTROL_PIN = 9;
const int NUM_AVG = 5; //determines how many readings are taken at each angle of servo
const int NUM_READINGS = 13; //
const int NUM_ANGLES = NUM_READINGS - 1;  //One reading at zero, one reading at 180
                                          //NUM_ANGLES must divide 180 evenly
//TODO: play with "degPerSecond"
const float DEG_PER_SECOND = 360.0; //This constant denotes how many degrees per second the robot can turn
                                   //given a certain wheel speed. Empirically determined.                                  
float distance[NUM_READINGS];

//Robot control constants and variables
const int DEFAULT_SPEED = 125; //do not use negative values
const int DEFAULT_ACTIVE_TIME = 500; //default "on time" of DC motors in milliseconds,
                                     //could be re-written in terms of speed.

//Function prototypes for Arduino
int scanPath();
void steerRover();
void roverForward();


void setup() 
{
  //Initialize servo object to pin 9 (may not be required with Adafruit motor shield)
  rangeServo.attach(SERVO_CONTROL_PIN);
  
  //Connect to motor controller
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
  
  //Initialize serial port for debugging/future Xbee
  Serial.begin(9600);
}

void loop() 
{
  steerRover(scanPath());
  
  //testing if rover will steer using "steerRover" function
  //steerRover(45); //At 75 deg rover executes a slight left hand turn
                  //left wheels spin visibly slower than right wheels
  delay(1000);
  roverForward(); //"roverForward" function working
  delay(1000);

}

//----------------------------------------------------------------------------------------------------
//Function for sweeping the rangefinder, returns the angular position of the clearest path
int scanPath()
{
  long duration = 0;
  float tempDist[NUM_AVG]; //array to store all readings from a given angle for averaging
  
  //Loop through the different measurement angles for the rangefinder
  for(int i = 0; i < NUM_READINGS; i++)
  {
    float tempAvg = 0;
    rangeServo.write(i*(180/NUM_ANGLES));
    delay(50);
    
    //Take multiple readings from rangefinder, store them in "temp" array
    for(int j = 0; j < NUM_AVG; j++)
    {
      //Send trigger signal to rangefinder
      sendTrigger(RANGE_TRIG_PIN, 5);
      //Detect reflected pulse and time of flight
      duration = pulseIn(RANGE_SIG_PIN, HIGH, 30000); //30,000 microseconds, limit range to ~500cm
      //Calculate distance travelled by pulse-echo wave
      tempDist[j] = (duration / 29.1545) / 2.0; //divide by two due to pulse-echo (assumed v=343 m/s)
      delay(30);
    }
    
    //Average the readings from temp for a given angle, store them in distance
    distance[i] = avgFloat(tempDist, NUM_AVG);
  }
  
  //Return the angle at which the largest rangefinder reading was found
  return (indexMaxFloat(distance, NUM_READINGS)*(180/NUM_ANGLES));
}

//----------------------------------------------------------------------------------------------------
//Function for steering/orienting the rover towards the clearest path
void steerRover(int pathAngle)
{


  float timeActive = (abs(90-pathAngle))/DEG_PER_SECOND; //calculate how long the motors should be on to 
                                                       //deviate a certain angle from straight (90 deg)
  timeActive = timeActive*1000.0; //convert to milliseconds
  
  float pathAngleRad = (pathAngle*(PI/180.0))/2.0; //divide by 2 to scale between 0-90 deg (avoid negative trig stuff)
  
  //Left wheels are mapped using sin and right using cosine, more elegant than "skid steering"
  flMotor->setSpeed((int)(DEFAULT_SPEED*sin(pathAngleRad)));
  frMotor->setSpeed((int)(DEFAULT_SPEED*cos(pathAngleRad)));
  blMotor->setSpeed((int)(DEFAULT_SPEED*sin(pathAngleRad)));
  brMotor->setSpeed((int)(DEFAULT_SPEED*cos(pathAngleRad)));
  
  //Debugging only
  Serial.print("Front left: ");
  Serial.println(sin(pathAngleRad));
  Serial.print("Front right: ");
  Serial.println(cos(pathAngleRad));
  Serial.print("Back left: ");
  Serial.println(sin(pathAngleRad));
  Serial.print("Back right: ");
  Serial.println(sin(pathAngleRad));
  
  
  flMotor->run(FORWARD);
  frMotor->run(FORWARD);
  blMotor->run(FORWARD);
  brMotor->run(FORWARD);

  delay(timeActive);

  
  
  flMotor->run(RELEASE);
  frMotor->run(RELEASE);
  blMotor->run(RELEASE);
  brMotor->run(RELEASE);
  
}

//----------------------------------------------------------------------------------------------------
//Function for moving the rover forward a small distance. All wheels spin at the same speed.
void roverForward()
{
  flMotor->setSpeed(DEFAULT_SPEED);
  frMotor->setSpeed(DEFAULT_SPEED);
  blMotor->setSpeed(DEFAULT_SPEED);
  brMotor->setSpeed(DEFAULT_SPEED);
  
  //Move rover forward a given distance determined by "DEFAULT_ACTIVE_TIME"
  flMotor->run(FORWARD);
  frMotor->run(FORWARD);
  blMotor->run(FORWARD);
  brMotor->run(FORWARD);
  
  delay(DEFAULT_ACTIVE_TIME);
  
  flMotor->run(RELEASE);
  frMotor->run(RELEASE);
  blMotor->run(RELEASE);
  brMotor->run(RELEASE);
  
}
  
//----------------------------------------------------------------------------------------------------
  
  
