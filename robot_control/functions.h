#include "Arduino.h"


//Trigger signal function, sends a square wave (digital HIGH) signal with a length of 
//"pulseLength" microseconds to the "trigPin" pin (must already be intialized as output).
void sendTrigger(int trigPin, long pulseLength)
{
  //set trigPin low to ensure clean high pulse
  digitalWrite(trigPin, LOW);
  delay(3);
  digitalWrite(trigPin, HIGH);
  delay(pulseLength);
  digitalWrite(trigPin, LOW);
}

//Function for calculating the average (mean) of an array of floats "values" of length "arrLength". 
//Loss of precision may occur.

float avgFloat(float values[], int arrLength)
{
  float temp = 0;
  
  for(int i = 0; i < arrLength; i++)
  {
    temp += values[i];
  }
  
  return temp/(arrLength*1.0);
}

//Function for finding the largest value in an array, returns index of largest element
int indexMaxFloat (float values[], int arrLength)
{
  int maxVal = values[0];
  int indexMaxVal = 0;
  
  for(int i = 0; i < arrLength; i++)
  {
    if(maxVal < values[i])
    {
      maxVal = values[i];
      indexMaxVal = i;
    }
  }
  
  return indexMaxVal;
}


