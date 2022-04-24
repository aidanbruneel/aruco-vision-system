#include "Wire.h"
#include "Adafruit_DRV2605.h"
#include "math.h"
#define TCAADDR 0x70
extern "C" {
#include "utility/twi.h"  // from Wire library, so we can do bus scanning
}

  int northPin = 11;
  int westPin = 10;
  int eastPin = 9;
  int off = 100;
  int baseBpm = 100;
  int standardDistance = 5;
  
void setup() {
  // put your setup code here, to run once:
  //MOVED FROM OUTSIDE SETUP TO INSIDE |--->
  Adafruit_DRV2605 drv;
 
  // <---|
  pinMode(eastPin, OUTPUT);
  pinMode(northPin, OUTPUT);
  pinMode(westPin, OUTPUT);
  pinMode (0, INPUT);
  pinMode (1, INPUT);
  pinMode (2, INPUT);
  pinMode (3, INPUT);

  Serial.begin(9600);
  drv.begin();
  drv.setMode(DRV2605_MODE_PWMANALOG);

  // ac coupled input, puts in 0.9V bias
  drv.writeRegister8(DRV2605_REG_CONTROL1, 0x20);

  // analog input
  drv.writeRegister8(DRV2605_REG_CONTROL3, 0xA3);
  Serial.print("In setup");
  Serial.println();

  // turn them all off to start
  analogWrite(eastPin, off);
  analogWrite(northPin, off);
  analogWrite(westPin, off);
}

int determineDelay(double bpm, double distance, double unalteredDistance, double noteDelayDenominator){//Bpm, distance in meters, distance where bpm= stays the same, and the factor which the delay is going to be divided by. 1=quarter, 2=eighth. 

double distanceDiff = unalteredDistance - distance;
double bpmScale = 50*distanceDiff;
double bpmTotal = bpm+bpmScale;
double beats = 60000/bpmTotal;
double delayItNow = beats/noteDelayDenominator;

Serial.print(distanceDiff);
Serial.println();
Serial.print(bpmScale);
Serial.println();
Serial.print(bpmTotal);
Serial.println();
Serial.print(beats);
Serial.println();
Serial.print(delayItNow);
Serial.println();
return (int)delayItNow;

//return ((60, 000 / bpm) + (bpmScalingFactor * (unalteredDistance-distance))/noteDelayDenominator);
}

int determineDelay(int bpm, int distance, int unalteredDistance){ //Bpm, distance in meters, distance where bpm= stays the same. QuarterNote unless extra param is specified. {
int bpmScalingFactor = 100;
return ((60, 000 / bpm) + (bpmScalingFactor * (unalteredDistance - distance)));
}

int compassControl(int onIntensity, double angle, int distance, int currentTime) {
  //angle in degrees, distance in cm, currentTime in ms
  int tunedDelay = determineDelay(baseBpm, distance, standardDistance,2);
  Serial.print("Tuned Delay: ");
  Serial.print(tunedDelay);
  Serial.println();
  String comDirection = determineDirection(angle);
  Serial.print("In compass");
  Serial.println();

  for (int i = 0; i < comDirection.length(); i++)
  {
    Serial.print(comDirection.charAt(i));
    Serial.println();
    switch (comDirection.charAt(i)) {
      case 'N':
        pulseMotor(northPin,onIntensity,tunedDelay);
        delay(tunedDelay);
        break;
      case 'E':
        pulseMotor(eastPin,onIntensity,tunedDelay);
        delay(tunedDelay);
        break;
      case 'W':
        pulseMotor(westPin,onIntensity,tunedDelay);
        delay(tunedDelay);
        break;
      default:
        delay(2*tunedDelay);
        break;
    }
  }
 
}

void pulseMotor(int pin, int intensity, int dTime){
int realintensity = intensity*(155/100) + 100;
analogWrite(pin,realintensity);
delay(dTime);
analogWrite(pin,off);

}

String determineDirection(double angle) {
  //takes angle and determines closest direction.
  // ZERO FOR NOW IS TO TURN OFF. CHANGE BACK TO >= LATER.
  if ((angle > 0 && angle < 11.25) || (angle > 348.75 && angle < 360))
  {
    return "EEE_";
    Serial.print("Going EEE");
    Serial.println();
  } else if (angle >= 11.25 && angle < 33.75)
  {
    return "EEN_";
    Serial.print("Going EEN");
    Serial.println();
  } else if (angle >= 33.75 && angle < 56.25)
  {
    return "NE__";
    Serial.print("Going NE");
    Serial.println();
  } else if (angle >= 56.25 && angle < 78.75)
  {
    return "NNE_";
    Serial.print("Going NNE");
    Serial.println();
  } else if (angle > 78.75 && angle < 101.25)
  {
    return "NNN_";
    Serial.print("Going NNN");
    Serial.println();
  } else if (angle >= 101.25 && angle < 123.75)
  {
    return "NNW_";
    Serial.print("Going NNW");
    Serial.println();
  } else if (angle >= 123.75 && angle < 146.25)
  {
    return "NW__";
    Serial.print("Going NW");
    Serial.println();
  } else if (angle >= 146.25 && angle < 168.75)
  {
    return "WWN_";
    Serial.print("Going WWN");
    Serial.println();
  } else if (angle > 168.75 && angle < 270)
  {
    return "WWW_";
    Serial.print("Going WWW");
    Serial.println();
  } else if (angle >= 270 && angle < 348.75)
  {
    return "EEE_";
    Serial.print("Going EEE");
    Serial.println();
  }
  //If no given heading this error string will be returned
  return "XxxX";
}

void loop() {
  double inputAngle;
  
  int cTime = millis();
  int onIntensity=100;
  int distance;
  
  //take angle and distance
  //determine

  if (Serial.available() > 0) {
    inputAngle = Serial.parseInt();
    Serial.print("Input Angle: ");
    Serial.print(inputAngle);
    Serial.println();
    delay(1000);
  }
  if (Serial.available() > 0) {
    distance = Serial.parseInt();
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println();
    delay(1000);
  }
  Serial.print("I live.");
  Serial.println();
  compassControl(onIntensity, inputAngle, distance, cTime);

  //check battery charge
  checkCharge = digitalRead(0);
  
  if (checkCharge == HIGH){
    if (digitalRead(1)==LOW){
      // buzz once
      pulseMotor(northPin,onIntensity,tunedDelay);
      pulseMotor(eastPin,onIntensity,tunedDelay);
      pulseMotor(westPin,onIntensity,tunedDelay);
    }
    else if (digitalRead(3) == LOW){
      // buzz twice for fast charge
      pulseMotor(northPin,onIntensity,tunedDelay);
      pulseMotor(eastPin,onIntensity,tunedDelay);
      pulseMotor(westPin,onIntensity,tunedDelay);
      delay(50);
      pulseMotor(northPin,onIntensity,tunedDelay);
      pulseMotor(eastPin,onIntensity,tunedDelay);
      pulseMotor(westPin,onIntensity,tunedDelay);
    }
    else {
      // buzz three times for trickle charge
      pulseMotor(northPin,onIntensity,tunedDelay);
      pulseMotor(eastPin,onIntensity,tunedDelay);
      pulseMotor(westPin,onIntensity,tunedDelay);
      delay(50);
      pulseMotor(northPin,onIntensity,tunedDelay);
      pulseMotor(eastPin,onIntensity,tunedDelay);
      pulseMotor(westPin,onIntensity,tunedDelay);
      delay(50);
      pulseMotor(northPin,onIntensity,tunedDelay);
      pulseMotor(eastPin,onIntensity,tunedDelay);
      pulseMotor(westPin,onIntensity,tunedDelay);
      delay(50);
    }
  }
}
