/*
Title: Sweeping Servo Rangefinder
Hardware: -Ultrasonic range finder 
          -Servo motor attached to ultrasonic range finder for sweeping
          
Description: This code controls an ultrasonic range finder attached to a servo motor.
             The user specifies how many readings to take over the span of 180 deg, 
             as well as how many measurements should be averaged at each angle. The
             "safest" heading is then printed to serial, where "safest" is the angle
             at which the average distance from the rangefinder is maximal.

Author: Austin Hubbell

UPDATE LOG
-June 29, 2014: File creation, basic tests with Arduino Uno (appears to be working)

*/

#include <Servo.h>

//Ultrasonic rangefinder
const int RANGE_SIG = 4;
const int RANGE_TRIG = 5;
const int NUM_AVG = 5; //determines how many readings are taken at each angle of servo
const int NUM_READINGS = 13; //
const int NUM_ANGLES = NUM_READINGS - 1;  //one reading at zero, one reading at 180
                                          //NUM_ANGLES must divide 180 evenly
float distance[NUM_READINGS];

//Create servo object for the rangefinder
Servo rangeServo;
int rangeServoPos = 0;

//Robot control variables
int heading = 0;
//----------------------------------------------------------------------------------------------------
//Function for sweeping the rangefinder, returns the angular position of the clearest path
int scanPath()
{
  long duration = 0;
  float temp[NUM_AVG]; //array to store all readings from a given angle for averaging
  
  //loop through the different angles
  for(int i = 0; i < NUM_READINGS; i++)
  {
    float tempAvg = 0;
    rangeServo.write(i*(180/NUM_ANGLES));
    delay(50);
    
    //take multiple readings from rangefinder, store them in "temp" array
    for(int j = 0; j < NUM_AVG; j++)
    {
      //Send trigger signal to rangefinder
      digitalWrite(RANGE_TRIG, LOW);//write low first to ensure clean high pulse
      delay(2);
      digitalWrite(RANGE_TRIG, HIGH);//write high to trigger the rangefinder sonic pulse
      delay(5);  //TODO: check spatial resolution of rangefinder with this pulse length
      digitalWrite(RANGE_TRIG, LOW);//write low to end the rangefinder sonic pulse
      
      duration = pulseIn(RANGE_SIG, HIGH, 30000); //30,000 microseconds, limit range to ~500cm
      temp[j] = (duration / 29.1545) / 2.0; //divide by two due to pulse-echo (assumed v=343 m/s)
      delay(30);
    }
    
    //average the readings from temp for a given angle, store them in distance
    for(int i = 0; i < NUM_AVG; i++)
    {
      tempAvg += temp[i];
    }
    
    distance[i] = tempAvg/(NUM_AVG*1.0);
  }
  
  
  //Find the angle with the largest rangefinder reading
  float highVal = distance[0];
  float highIndex = 0;
  
  for(int i = 0; i < NUM_READINGS; i++)
  {
    if(distance[i] > highVal)
    {
      highVal = distance[i];
      highIndex = i;
    }
  }
  
  //return the angle at which the largest rangefinder reading was found
  return (highIndex*(180/NUM_ANGLES));
}
//----------------------------------------------------------------------------------------------------

void setup() 
{
 rangeServo.attach(9);
 
 pinMode(RANGE_SIG, INPUT);
 pinMode(RANGE_TRIG, OUTPUT);
 
 Serial.begin(9600);
}

void loop() 
{
  heading = scanPath();
  Serial.print("Clearest path: ");
  Serial.println(heading);
  delay(2000);

}
