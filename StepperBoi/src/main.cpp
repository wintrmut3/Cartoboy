//Includes the Arduino Stepper Library
#include <Stepper.h>
#include <Arduino.h>

// Defines the number of steps per rotation
const int stepsPerRevolution = 128;

// Creates an instance of stepper class
// Pins entered in sequence IN1-IN3-IN2-IN4 for proper step sequence
Stepper axisY = Stepper(stepsPerRevolution, 52, 48, 50, 46);
Stepper axisX = Stepper(stepsPerRevolution, 42, 38, 40, 36);
Stepper axisZ = Stepper(stepsPerRevolution, 32, 28, 30, 26);

void setup() {
	// Nothing to do (Stepper Library sets pins as outputs)
	axisX.setSpeed(250);
	axisY.setSpeed(250);
	axisZ.setSpeed(250);
  
  Serial.begin(9600);
}
int keystroke = 0;
int ssize = 20;
void loop() {
	// // Rotate CW slowly
	// myStepper.setSpeed(100);
	// myStepper.step(stepsPerRevolution);
	// delay(1000);
	
  if (Serial.available()>0){
    keystroke = Serial.read();
    Serial.print("ks ");
    Serial.println(keystroke);
  }
  if (keystroke == 'a') axisX.step(+ssize);
  else if (keystroke == 'd') axisX.step(-ssize);
  else if (keystroke == 'w') axisY.step(+ssize);
  else if (keystroke == 's') axisY.step(-ssize);
  else if (keystroke == 'q') axisZ.step(+ssize);
  else if (keystroke == 'e') axisZ.step(-ssize);

  keystroke = 0;
	// Rotate CCW quickly
	// myStepper.step(2038);
	// delay(1000);
	// myStepper.step(-2038);
	// delay(1000);
}