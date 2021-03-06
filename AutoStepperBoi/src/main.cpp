#include <Arduino.h>
#include <Stepper.h>
#include "image.hpp"
#define DEBUG 1
const int maxDim = 312;
const int ssize = DEBUG ? 20 : 10; //step size multiplier
const int stepsPerRevolution = 128;
const int moveDelay = 20;
const int superstep_size = 6;


// Globals
int keystroke = 0;
int cx = 0, cy = 0, cz = 0;
bool manualPilot = DEBUG;

// Function Stubs
void move(int dx, int dy);
void zmove(int dz);
void manualControls();
void autoControls();

// Creates an instance of stepper class
Stepper axisY = Stepper(stepsPerRevolution, 52, 48, 50, 46);
Stepper axisX = Stepper(stepsPerRevolution, 42, 38, 40, 36);
Stepper axisZ = Stepper(stepsPerRevolution, 32, 28, 30, 26);

// Main Code

void setup() {
	// Nothing to do (Stepper Library sets pins as outputs)
	axisX.setSpeed(250);
	axisY.setSpeed(250);
	axisZ.setSpeed(250);
  
  Serial.begin(9600);
  delay(1000); //smon
}

void loop() {
  manualControls();
  if(!DEBUG) autoControls();
}

void move(int dx, int dy){
  // need to normalize and calibrate
  if(!manualPilot){
    if(cx+dx < 0) dx = -cx;
    if(cy+dy < 0) dy = -cy;
    if(cx+dx > maxDim) dx = maxDim-cx;
    if(cy+dy > maxDim) dy = maxDim-cy;
    delay(moveDelay);

    if(dx > superstep_size){
      delay(dx/superstep_size * moveDelay);
    }
  }


  axisX.step((12 * -dx * ssize) / 10); //eg. normalize
  // axisX.step(-dx * ssize)
  axisY.step(dy * ssize); 

  // Serial.println(dx);

  cx += dx;
  cy += dy;
}

void zmove(int dz){
  const int jumpSize = 48;
  if(manualPilot){
    axisZ.step((2*dz-1) * ssize);
    return;
  }

  if(dz > 0 && cz == 0){
    axisZ.step(ssize * jumpSize);
    cz = ssize*10;
    delay(moveDelay*4);
  }

  else if(dz == 0 && cz > 0){
    axisZ.step(-ssize * jumpSize); // move to zero
    cz = 0;
    delay(moveDelay*4);
  }

  else {
    Serial.println("Ignored invalid zmove.");
  }
}

void manualControls(){
  if (Serial.available()>0){
    keystroke = Serial.read();
  }
  if (keystroke == 'a') move(-1,0);
  else if (keystroke == 'd') move(1,0);
  else if (keystroke == 'w') move(0,1);
  else if (keystroke == 's') move(0, -1);
  else if (keystroke == 'q') zmove(1);
  else if (keystroke == 'e') zmove(0);  
  else if (keystroke == 'm') manualPilot = !manualPilot;
  else if (keystroke == 'o'){
    // autozero
    move(-cx, -cy);
    Serial.println("autocentering");
  }

  
  //calibrate (zero the coords)
  if (keystroke == 'z') {
    cx = 0; 
    cy = 0; 
    cz = 0;
  }

  if(keystroke!=0){
    Serial.print("[ ");
    Serial.print(cx);
    Serial.print("\t");
    Serial.print(cy);
    Serial.print("\t");
    Serial.print(cz);
    Serial.print (" ] ");
    Serial.println(manualPilot?"M":"A");
  }
  keystroke = 0;
}

int autoctr = 0;
void autoControls(){
  if(autoctr > 0){
    move(-cx, -cy);
    exit(0);
  }
 
  // 64x64
  // char gcode[] = "URDRRURRRRRRRDRRRRRRRURDRRURRRRRDRURRRRRRRRRRRRRRRRRDRRRURRRRRRRRRRRRRDRRURRDRRURRRRRRDRTDLLLLULLLDLLULLLLLDLLLULLLLLLLLLLDLLULLLLLLLLLLLLLLLLLLLLLLLDLULLLLLDLLLLLLLULLLLDLLULTURRDRRRRRRRRRURDRURRRRRRDRRURRRRRDRURRRRRRRRRRRRRRRRDRRURRRRRRRRRRRRRRRRRRRDRRRURRRRRTULLLLDLULDLLULLLLLLLLLLLLLLLLLLLDLULLLLLLLLLLLLLLLLLDLULLLLLDLLULLLLLLLLLLLLDLLLLLLLTDRRRRURRRRRRRRRDRURRRRRRDRURRRRRRRRRRRRRRRRRRRRRRRDRRURRRRRRRRRRRRRRRRRRDRRURDRURDRURDRTULLLLLLDLULLLLLLLLLLLLLLLLLLLDLLULLLLLLLLLLLLLLLLDLULLLLLDLLULLLLLLDLULLLLLLLLDLLULLLTURRRDRRRURRRRRRRDRURRRRRRDRRURRRRRDRURRRRRRRRRRRRRRRRDRRRRRURRRRRRRRRRRRRRRRDRRURRRRRTULLLLLDLLULLLLLLLLLLLLLLDLLLLLLLULLLLLLLLLLLLLLLLDLULLLLLDLLULLLLLDLLULLLLLLLDLLULLLLTURRRRDRRRURRRDRRRURDRURRRRRRDRURRRRRRRRRRRRRRRRRRRRRDRRURRRRRDRRURRRRRRRRRRRRRRDRURRDRRURTULLLLDLLLLLULLLLLLLLLLDLLULLLLLLLDLLULLLLLLLLLLLLLDLULLLLLDLLULLLLLLDLULLLLLDLLLLLULLLLTURRRRRDRRURRRRRRRRRRRRRRDRRURRRRRDRURRRRRRRRRRRRRDRRURRRRRRRDRRURRRRRRRDRRRRRRRRURRRRTDLULLLDLLULLLDLLLLLLULLLLDLULLLLLLLLLDLULLLLLLLLLLLLLLLLLLLDLULLDLLLULDLULLLLLLLDLLLULLLLLTURRRRRDRRRURRRRRRRDRRURRRRRDRRURRRRRDRURRRRRRRRRDRRRRURRRRRRRRRDRRRRRRRURRRRRRRRDRRRURRTULLLDLLULLLLLLLLLLLDLLLLULLLLLLLLLDLLLLLLLULLLLLLDLULLLLLDLLULLLLLLDLULLDLLLULLDLLULLLLLLTURRRRRRDRRRURRRRRRRDRURRRRRDRRURRRRRRRRDRRRRRRRURRRDRRURRRRRRRRRDRURRRRRRRRRRRRRDRRURRRTDLLLLULLLLLLLLLLLLLDLLULLLLLLLLLDLLULLLLLLDLLLLLLLULLLLLDLULLLLLLDLULLLLLLLDLLLLLLULLLTDRRURRRRRDRRURRRRRRRDRURRRRRRDRRRRRRRRURRRRDRURRRRRRDRRURRRRRRRRRDRRURRRRRRRRRRDRRRRRRURTULLDLLLLLLLLULLLLLLLDLLULLLLLLLLLDLULLLLLLDLULLLLLLLLDLLLLLULLLLLLDLULDLLLULLLDLLULLLLLLDLTDRURRRRRRDRRRRRURRRRRDRURRRRDRRRURRRRRRRRRRRDRURRRRRRDRRURRRRRRRRRDRRURRRDRRRRRURRRRRDRURRTULDLLULLLLLLLLDLLLLLLLULLLLLLLLLDLLULLLLLLDLULLLLLLLLLLDLLLLLLULLDLULLLLLLLDLLLULLLLLDLULTURDRURRRRRRDRRURRRRRRRDRRRRRURRDRRURRRRRRRRRRRRRRRDRURRDRURRRRRRRDRRRRRRURRRRRRRRRRRDRRURTDLLULLLLLLLLLLLLLLDLLLLLLLULLLLDLULDLULLDLLLULLLLLLLLLLLDLLULLLLLDLLLLLLULLDLLLULLLLLLLLTURRDRURRRRRDRRRRRRRURRRDRURRRRRRDRRURRRRRRRRRRDRURRRRRDRRRRRRRRURRRRDRRURRRRRRRRRRRRRDRRTDLLULLLLLLLLLLLLLDLLULLLLLLLDLLLLLULLLLLLLLLLLLLLLLDLLULLLLLLDLULLLLLLDLLLLLLULLLDLULLTURRDRRRRRRRRRURRRRRRRRRRRRRRDRRURRRRRRRRRRRDRURRRDRURRDRURRRRRRRRRRDRURRRRRRRRRRRRRRDRTDLULLLLLLLLLLLLLDLLULLLLLLLLLDLLULLLDLLULDLULLLLLLLLLLLDLULLLLLLDLULLLLLLLDLLLULLLLDLLLLLTDRURRDRURRRRRDRRRURRRRRRRDRURRRRRRDRRURRRRRRRRRRDRRURRRRRDRRURRRRRRRRRDRRURRRRRRRRRRRDRRRTDLLLLLLULLLLLLLLDLLULLLLLLLLDLLULLLLLLDLULLDLLLULLLLLLDLLULLLLLLLLLLLLLLDLLULLLLLLDLULLLTURRRDRURRRRRRDRRRURRRRRRRRRRRRRRDRURRRRDRRRURRRRRDRURRRRRDRRURRRRRRRRRDRURRRRRDRRRRRURRRRTULLLLLLLDLLLLLLLLULLLLLLLLDLLULLLLLDLLULLLLLLDLLULLLDLLULLLLLLDLULLLLLLLDLLLULLLLLLLLLLTDRURRRDRURRRRRRDRRURRRRRRRDRURRRRRRDRRURRRRDRURRRRRRRDRURRRRRDRRURRRRRRRRDRRRRRURDRURRRRRRRRTULLLLLLLDLLULLLLDLLULLLLLLLLDLULLLLLLDLULLLLLLLDLULLLLDLULLLLLLLDLULDLLULLLDLLLULLLLLLDLULLLDLTDRURRRRRRRRRRDRRRRRURRRRRDRURRRRRRDRRURRRDRURRRRRRRDRURRRRRRDRRURRRRRRRDRURRRRRDRRURRRRDRRRTULLLLDLLLLULLLLLLDLLULLLLLLDLULLLLLLLDLLLULLLLDLULLLLDLLULLLLLLDLULLLLLLLDLLLLULLLLDLULLLDLULTURDRURRRDRRRRURRRDRRURRRRRRRRRRRRRRDRRURRRRDRURDRRRURRRRRRRRRRDRRURRRRRDRRURRRRRRDRRRRURRRRTULLLLDLULLLLLLLLLLDLLULLDLLLULLLLLLLLLLLDLULLLLLLLLDLULLLLLLDLLULLLLLLDLLLULLLLLDLLULLLDLULTDRRURRRRDRURRRRRDRRRURRDRRRURRDRURRRRRRDRRURRRRRRDRRURRRRRRRRRRRRDRRRRRURRRRRRRRRRRDRRRRRTDLLLLLLLLULLLLLLLLDLLLULLLLLLLLLLLLLDLULLDLULLLLLDLLULLLLLLDLULLLLLDLLLLULLLLLLDLULLLDLULLTURRDRURRRDRURRRRRRDRRRRRURRRRRRRRDRRURDRRURRDRRURRRRDRURRRRRRRRRRRRRRDRRURRRRRDRRRRRRRRURRDRTDLULLDLULLLLDLLLLLLULDLLLULLLLLLLLLLLLLLLLLLLLLLDLLULLLLDLLLLULLLDLLLULLDLLULLLLLLLLLDLULLTURRRDRURRRDRURRRDRRURRRRDRRRRRRURRRRRRRDRRURRRRRRRRDRURRRRRRRRRRRRRDRRRRRRURRRRRRRRDRRURRTULLDLLULLLLLLLLLLDLLLLLLLULLLLLLLLLLDLULLLLLLLLDLULLLLLLLLLLLLDLULLLLLDLLULLLDLULLLDLULLLTURRRDRURRRRRRRDRRURRRRRDRURRRRDRURRRRRRRDRRURRRRRRRDRURRRRRRRDRRRRRRURDRRRURRRRRRRRRRRDRURRTULDLLULLLLLLLLLLLDLLLULLLLLDLLLLLLULLLLLLLLLLLDLLULLLLLLLDLULLLLDLLULLLLDLLLLLULLLLLLLLTURRRRDRURRRDRURRRDRRRURDRRURRRRRRRRRRRRRDRRURRRRRRRDRRRRRRRURRRRRRRRRDRRURRRRRRRRRRRDRRURTULDLULLLLLLLLLLLDLLLULLLLLLLLLLLDLLLLLLLLULLLDLLULLLLLLLDLULLLLLLDLLLLLULLLLDLULLLDLULLLLTURRRRRRRRRDRURRRRRDRRRURRRRRRDRURRRRRRRDRRRRRRRRURRRRDRURRRRRRRRRRRDRRRURRRRRRRRRRRDRRTDLLULLLLLLLLLLLDLLLULLLLLLLLLLDLLULLLLDLULLLDLLLLLLULLLLLDLULLLLLLDLLULLLLLLDLULLLLLLLLDLTDRURRRRDRURRRDRURRRRRRDRRURRRRRRRDRRRRRRRRURDRRURRRDRURRRRDRRURRRRRRRDRRURRDRRRURRRRRRRRRRRDRTDLULLLLLLLLLLLDLLLULLLLDLLLLULLLDLLULLLDLULLLLDLLULLLDLULDLLLLLLULLLLDLLLULLLLLLLLLLDLULLLLDLTURRRRRRRRRRDRURRRRRDRRRRRRRRRRURRRRDRURRRRDRURRRRDRURRRRDRRRRURRRDRURRRRDRRRURRRRRRRDRRRRRTULLDLLLLLLULLLDLLLULLLLLDLULLLLLDLLULLLLDLULLLDLLULLLLLLLLLLLLLDLLLLLLLLULLLDLULLLDLULLLLDLULTURDRURRRRDRURRRDRRRRRRURDRRRURRRRRRDRURRRRDRURRRDRRURRRRDRURRRDRRURRRRRRDRURRRRDRRRRRRRRRURRRRRTULLLLLLLLLDLLLLLULLLLDLULLLLLLDLULLLLDLULLLLDLULLLLDLULLLLDLULLLLLLDLLLULLLLLDLLLLLLULLLLDLULTURDRRRRRRRRURRRRRRRRRDRRURRRRRRDRURRRRDRURRRRDRRURRRDRURRRRDRRURRRRRDRURRRRRDRRRURRRRRRRRRRTULLLLLLLLLLDLLLULLLLDLULLLLLLDLLULLLDLLLULLDLLULLLLLLLLDLULLLDLULLLDLLULLLLLLLLLLLLDLLLLLLTDRRURRRRDRURRRRRRRRRRRDRRRURRDRURRRDRURRRRDRURRRDRRRRURRDRURRRRDRURRRRRRDRURRRRDRRRURRRRRRRRRRTDLLLLULLLLLLDLLLULLLLDLULLLLLDLLULLLLDLULLLLDLLLLLLULLLLDLULLLLLLDLLLULLLLLLLLLLLDLULLLLLDLTURRRRRRDRURRDRRURRRRRRDRRRRURRRDRURRRDRURDRRRURRRRDRRURRRDRURRRRDRRURRRRRRDRURRRRDRRRURRDRRRRRURRTULLLLLDLLLLLLLULLLLDLULLLLLLDLLULLLDLULLLLDLLULLLLLLLLDLLLULDLULLDLLLULLLDLLLULLLLLLLLLLLDLULTURDRURRRRRDRURRDRRURRRRRRRDRRRURDRRRURRDRURRRRRRRRRDRURRRRDRURRRDRRRURRRRRDRRRRRRRRRURRRRRRRRTULLLLLLLLDLLLULDLLLLLLLLULLDLLULDLLLLULLLDLLULLLLLLLLLDLULLLLLDLLLULLLLLLLLLLLDLLLULLLLLLTURDRRRRURRRRRRRRRRRRRRRDRRRURRRRRRDRURRRRRRRRDRRURDRRRURRRRDRRRRRRRURRRDRURRDRRRURRRRRRRDRTULLDLLLULLLDLLLULDLULLLLDLULLLDLLULLLLLLLLLDLLLULLLLLLLDLULLLLLLDLLLLLLULLLLLLLLDLULLLLLDLLLTURRDRURRRRRDRURRRRRDRRRURRRDRRRURRRRRRDRURRRRDRRRURRDRRURRRRRRRRDRRURRRDRURRRRDRURRDRRRRURRDRURRRTULLLDLULLLDLLLULLDLULLLLLLLLDLLULLLLLLLLDLLULLLLLLDLLLLULLLLLLDLLULLLLLLLLDLLLLLULLLLDLULLLTURRRDRURRRDRRRURRRRRRRRRRRDRRRURRRDRRRRURRRRRRRRDRRURRRRRRDRRRRURRRRDRURRRRRRDRRRURRRDRRRRTDLLLLLLLLLULLDLULLLLDLULLLDLLULLLLLDLLULLDLULLLLLLLLLDLULLLLLDLLLLLLULLLLLLLLLDLULLLDLLLULLLTDRURRDRURRRRRDRURRRRRDRRRURRRRDRRURRRRRRRRRRRRRRRRDRRRURRRRRRRDRRURRRRRRRRRRRDRRRRRURRRRTULLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLDLLLULLLLDLULLLLLLDLLLULLLLLLLLLLLLLLLLLDLULLLLTURRRRDRURRRRRRRRRRRRRRRRRDRRRURRRRRRDRURDRRRURRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRTULLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLDLLLLULLLLDLLLLLLULLDLULLLLLDLULLLLLDLULLLLT";
  // char gcode [] = "DRURRRDRURRRRRRRRDRRURRRRRRRRRRRRDRRURRRRDRURRRRRRRRRRRRRRDRRURRRRRRRRRRDRURRRTULDLLLLULLLLLLLLLDLLULLLLLLLLLLLLLLDLULLLLDLLULLLLLDLLLULDLLLLLULLLLLDLULLDLULLLLTURRRRRRRDRURRRRRDRRURRRRRRDRURRRRRDRRURRRRRRRRRRRRRRRRRRRDRRURRRRRRRRRRRRRRTULLLDLULLLLLLLLLLDLLULLLLLLLLLLLLLLLLLLLDLLULLLLLLLLLLLLDLLULLLLLDLULLLLLLLTDRURRDRRURRDRURRRRRDRRURRRRRRRRRRRRDRRURRRRDRURRRRRRRRRRRRRRDRRRRRRRRRRRRRRRRTDLLLLLLLLLULLLLLLDLULLLLLLLLLLLLLLDLULLLLDLLULLLLLDLULLLLLLDLLULLDLLULDLLLLLLULDLTURRRRDRURRDRURRRRRDRRURRDRRURRDRRRURRDRRURRRRRDRURRRRRRRRRRRRRRDRURRRRRRRRRRRRRRRTULLLLLLLLLLLLLLLDLULLLLLLLLLLLLLLLLLLLLDLULLLLLLLLLLLLDLLULLLLLDLULLLLLLLTURRRRRRRDRURRRRRDRRURRRRRRRRRRRRDRURRRRRRRRRRRRRRRRRRRRDRURRRRRRRRRRRRRRRTULLDLLULLLLLLLLLLDLLLULLLLLLLLLLLLLDLULLLLLDLULLLLLDLULLLLDLLLLULLLLLLLLLDLULLLTURRRDRURRRDRURRRRRDRRURRRRDRRRURRRRRDRURRRRRDRURRRRRRRRRRRRDRRRRRURRRRRRRRRRRRRTULLLLLLLLLLLLDLLULLDLLLULLLLLLLLLLLDLULLLLLDLULLLLLLLLLLLLDLLULLLLLDLULLLLLLLTDRRRRRRRRURRRRDRRRURRRRRRRRRRRRDRRRURRRRRRRRRRRRRRDRRURRRRRDRRURRRRRRRRRRRTULLLLLLLLLLDLLULLLLLLLDLLLLLLLLLLLLLLLLLLLLULLDLLLLLLLLLLLLLLLLLLLLLLLTURRRDRURRRDRURRRRDRRRRRRRRRRRRRRRRURRRRRRRRRDRRRRRRRRURRRRRRRDRRURRRRRRRRRRTULLLLLLLLLLDLLULLLLLLLDLLULLLLLDLULLLLLLLLLDLULLLLLLLLLLLLDLLLULLLLDLULLLLLLLTURRRRRRRDRURRRRDRRRURRRRRRRRRRRRDRURRRRRRRRRDRURRRRRDRRURRRRRRRDRRURRRRRRRRRRTULLLLLLDLLLLLLULLLLLLLDLLULLLLLLLLLLLLLLLDLLULLLLDLLULLLLLDLLLULLLLLLLLDLULLLTURRRDRURRRRRRRRDRRRURRRRRDRRURRRRDRRURRRRRRRRRRRRRRRDRURRRRRRRRDRRRRRRRRRRRRTULLLLLLLLLLDLLULLLLLLLLDLULLLLLLLLLLLLLLLDLLULLLLLLLLLLLDLLLULLLLLDLULLDLULLLTURRRRRRDRURRRRRDRRRURRRRRRRRRRRDRRURRRRRRRRRRDRRURDRURDRURRRRRRRRDRRURRRRRRRRRRTULLLLLLLLLLDLLULLLLLLLLDLULDLULLLLLLLLLLLLLDLLULLLLLDLULLLLLLDLLULLLLLDLULLDLLULLTURRDRRURRRDRURRDRRRRRRRURRRDRURRRRRDRRURRRRRRRRRDRURRRRRDRURRRRRRRRDRURRRRRRRRRRRTULLLLLLLLLLLDLULLLLLLDLLLLLULLLDLULLLLLLLLLDLLULLLLLDLLULLLLDLLLULLLLLLLLDLULLLTURRRRRRDRURRRRRDRRURRRRRRRRRRRRDRRURRRDRRRURDRRRRURRRDRRRRRRRRRRRRRRRRRRRRRRTDLLLLLLLULLLDLLULLLLLLLLDLULLLLLDLULLLLDLLULLLDLLULLLLLDLULLLLLLDLLULLLLLDLULLLLLLTURRRDRURRRRRRRRDRRURRRRRRDRURRRRRDRRURRRDRURRRRRRRRRRRDRURRRRRRRRDRURRRRRRRRRRRTULLLLLLLLLLLDLULLLLLLLLDLULLLLLLLLLLLDLULLLDLLULLLLLDLLLULLDLLLLLULLLLLLLDLULLLTURRRRRRDRURRRDRRRURDRRURRRRRRRRRRDRRURRRRRRRRRRDRURRRRDRURRRRRRRRDRURRRRRRRRRRRTULLLLLLLLLLLDLULLLLLLLDLLULLLLLLLLLLLLLLLDLLULLLLLDLULLLLDLULLLLDLLULLDLULLLLLLTURRDRURRRDRRRRRURRRRDRRURRRDRURRRRRDRURRRRDRRURRRRDRURRRRRDRRURRRRRRRDRURRRRRRRRRRRTULLLLLLLLLLLDLULLLLLLLDLLULLLLLLDLLLULLLLLLLDLLLLULDLLLULLDLLULLLLDLLULLLLLLDLULLTURRRRRRRRRRDRRURRDRRURRRDRURRRRRRDRURRRRRRDRRURRRRRRRRDRRURRRRRRRDRURRRRRRRRRRRTULLLLLDLLLLLLLLULLLLLLDLLULLLLLLLLLLLLDLLLLLULLLLLLLLLLLDLLLLLULLLDLULLLLLLTURRDRURRRDRURRRRDRRRURRRRRRRRRRRRDRURRRRRRRDRURRRRRRRRDRRURRRRRRDRRRRRRRRRRRRRTULLLLLLDLLULLLLDLLULLLLDLLULLLLLLLLLDLULLLLLLLDLULLLLLLDLULLLLLDLLLULLLLDLULLLDLULLTURRDRURRRRRRRRDRRRURRRRRDRURRRRRRDRURRRRRRRDRURRRRRRRRRDRRURRRDRRURRRRRDRRURRRRRRTULLLLLLDLLULLLLLLDLLLLLULLLLLLLLLLDLULLLLLLLDLULLLLLLDLULLLLLDLLLULLLLLLLLLLLTDRRRRRRRRRURRDRRRURRRRRRRRRRRRDRURRRRRRRRRRRRRRRRRRRDRRRURRRRRRRDRRRURDRRURRTULLLLDLLLULLLLLLLLDLLLULLLLLLLLLLLLLLLLLLLDLULLLLLLDLULDLLLLLLLLLLLLLLLLLLTDRURRRRRRRRRRDRRRRRRRRRRRRRRRRRRRRRRRRURRRRRRRRRRRDRRRURRRRRRRRRDRURRRRRTULLLLLDLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLULLLDLULLLLLDLLLULLLLLLLLLLDLTURRRRRRRRRRRDRRURRRRRRDRURRRRRRRDRURRRDRURRRDRRURRRRRDRRRRRRRRRRRRRRRRRRRRRRTDLULDLULLDLULLLLLLLLLDLLLULLLLLLLLLDLLULLLDLULLLDLULLLLLLLLLLLLLLDLLULLLLLLLDLULLLTDRURRDRRURRRRRRDRRURRDRURRRRRRRDRURRRDRURRRDRURRRDRRURRRRRRRRRDRRURRRRRRRRRRDRURRDRURDRTDLULLLLDLULLLLLLLLLLDLLULLLLLLLLLDLLULLLLLLLDLULLLDLULLLLDLULLDLULLDLLLLULDLLULDLLLULDLTDRURRRRRRRRRRDRRURRDRURRDRURRRRRRRRDRURRRRRRRDRRURRRRRRRRRDRRURRRRRRRRRRDRURRRRDRTDLULLLLDLLULLLLLLLLLDLLLLLLLULDLLULDLLULLLDLULLLDLULLLLLLLLLLLLLLDLLULLLLLLLLLLLTURRRRRRRRRRRDRRURRDRURRRRRRRRRRDRRURRRDRURRRDRRURRRRDRURRRRDRRURRRRRRRRRDRRURDRURRDRTDLULLDLULDLLULLLLLLLLLDLLULLLLDLULLLLDLLULLLDLLULLDLLULLDLLULLDLLULLDLLULDLLULLLDLULLLDLULLDLTDRURRRRRDRRURRDRRRURRRRRDRURRRRRRRDRRURRDRRURRRDRRURRRRDRURRRRDRRURRRRRRRRRDRRURDRRRRTDLULLLLDLLULLLLLLLLLDLLULLLLDLULLLLDLLULLLLLLLDLLULLLLLLLDLULLLLLDLLULLLLLLLLLLDLTURRRRRRRRRRRDRRURRRRRDRURRRRRRRDRRURRRRRRRDRURRRRRDRURRRRDRRURRRRRRRRRDRRURRRRDRTDLULLLDLLLLLLLLLLLLLLULLLLDLULLLLLDLULLLDLULLLDLLULLLLLLLLLLLLLDLLLLULDLLULDLLLULDLTDRURRRRRRRRRDRRRURRRRRRRRRRRRRDRRURRRDRURRRDRURRRRRRRRRRDRRRRRRRRRRRRRRRRRRTDLULLLLDLLULLLLLLLLLDLLULLLLLLLLLLDLULLLDLULDLLLLLLLULDLLLLLLLULDLLLLULLLLLLLLLDLTURRRRDRRURRRRDRRRURRRRRDRURRRRRRRDRRRRURRDRRRRURRRRRRRRRRDRRURRRRRRRRRDRRRURRDRRTDLULLLLDLLULLLLLLLLLDLLLULLLDLULLLLLDLULLLLLLLDLLULLLLLLLDLULLLLLDLLLLLULDLLULLLLLTDRURRRRRRRRRDRRRURRRRRRRRRRRRRDRRURRRRRRRDRRRRRRRRRURDRRRURRRRRRRRRDRRURRRRDRTDLULLLLDLLLLLLLLLLLLLLLLLLLLLLLLULLLLLLLDLLULLLLLLLLLLLLLDLLLULLLLLDLULLLDLTDRURRRDRURRRRRDRRRURRDRRRRRRRURDRRRRRRRURRRRDRRURRRRRRRRRDRRRRRRRRRRRRRRRRRRRTULLLLLLLLLLLLLLLLLLLLLLLLLLLLDLLLLULLLLDLLULLLLLLLDLULLLLLLDLLULLLLLLLLLLTURRRRRRRRRRDRRURRRRRRDRURRRRRRRDRURRRRRRRDRRURRRRRRRRRRRRRRRRRRRRRRRRRRRRTULLLLLLLLLLLLLLLLLLLLLLLLLLLLDLLULLLLLLLDLULLLLLLLLLLLLLLDLLULLLLLDLULLLLT";
  // char gcode [] = "DR01UR03DR01UR08DR02UR12DR02UR04DR01UR14DR02UR10DR01UR03TUL01DL04UL09DL02UL14DL01UL04DL02UL05DL03UL01DL05UL05DL01UL02DL01UL04TUR07DR01UR05DR02UR06DR01UR05DR02UR19DR02UR14TUL03DL01UL10DL02UL19DL02UL12DL02UL05DL01UL07TDR01UR02DR02UR02DR01UR05DR02UR12DR02UR04DR01UR14DR16TDL09UL06DL01UL14DL01UL04DL02UL05DL01UL06DL02UL02DL02UL01DL06UL01DL01TUR04DR01UR02DR01UR05DR02UR02DR02UR02DR03UR02DR02UR05DR01UR14DR01UR15TUL15DL01UL20DL01UL12DL02UL05DL01UL07TUR07DR01UR05DR02UR12DR01UR20DR01UR15TUL02DL02UL10DL03UL13DL01UL05DL01UL05DL01UL04DL04UL09DL01UL03TUR03DR01UR03DR01UR05DR02UR04DR03UR05DR01UR05DR01UR12DR05UR13TUL12DL02UL02DL03UL11DL01UL05DL01UL12DL02UL05DL01UL07TDR08UR04DR03UR12DR03UR14DR02UR05DR02UR11TUL10DL02UL07DL20UL02DL23TUR03DR01UR03DR01UR04DR16UR09DR08UR07DR02UR10TUL10DL02UL07DL02UL05DL01UL09DL01UL12DL03UL04DL01UL07TUR07DR01UR04DR03UR12DR01UR09DR01UR05DR02UR07DR02UR10TUL06DL06UL07DL02UL15DL02UL04DL02UL05DL03UL08DL01UL03TUR03DR01UR08DR03UR05DR02UR04DR02UR15DR01UR08DR12TUL10DL02UL08DL01UL15DL02UL11DL03UL05DL01UL02DL01UL03TUR06DR01UR05DR03UR11DR02UR10DR02UR01DR01UR01DR01UR08DR02UR10TUL10DL02UL08DL01UL01DL01UL13DL02UL05DL01UL06DL02UL05DL01UL02DL02UL02TUR02DR02UR03DR01UR02DR07UR03DR01UR05DR02UR09DR01UR05DR01UR08DR01UR11TUL11DL01UL06DL05UL03DL01UL09DL02UL05DL02UL04DL03UL08DL01UL03TUR06DR01UR05DR02UR12DR02UR03DR03UR01DR04UR03DR22TDL07UL03DL02UL08DL01UL05DL01UL04DL02UL03DL02UL05DL01UL06DL02UL05DL01UL06TUR03DR01UR08DR02UR06DR01UR05DR02UR03DR01UR11DR01UR08DR01UR11TUL11DL01UL08DL01UL11DL01UL03DL02UL05DL03UL02DL05UL07DL01UL03TUR06DR01UR03DR03UR01DR02UR10DR02UR10DR01UR04DR01UR08DR01UR11TUL11DL01UL07DL02UL15DL02UL05DL01UL04DL01UL04DL02UL02DL01UL06TUR02DR01UR03DR05UR04DR02UR03DR01UR05DR01UR04DR02UR04DR01UR05DR02UR07DR01UR11TUL11DL01UL07DL02UL06DL03UL07DL04UL01DL03UL02DL02UL04DL02UL06DL01UL02TUR10DR02UR02DR02UR03DR01UR06DR01UR06DR02UR08DR02UR07DR01UR11TUL05DL08UL06DL02UL12DL05UL11DL05UL03DL01UL06TUR02DR01UR03DR01UR04DR03UR12DR01UR07DR01UR08DR02UR06DR13TUL06DL02UL04DL02UL04DL02UL09DL01UL07DL01UL06DL01UL05DL03UL04DL01UL03DL01UL02TUR02DR01UR08DR03UR05DR01UR06DR01UR07DR01UR09DR02UR03DR02UR05DR02UR06TUL06DL02UL06DL05UL10DL01UL07DL01UL06DL01UL05DL03UL11TDR09UR02DR03UR12DR01UR19DR03UR07DR03UR01DR02UR02TUL04DL03UL08DL03UL19DL01UL06DL01UL01DL18TDR01UR10DR24UR11DR03UR09DR01UR05TUL05DL36UL03DL01UL05DL03UL10DL01TUR11DR02UR06DR01UR07DR01UR03DR01UR03DR02UR05DR22TDL01UL01DL01UL02DL01UL09DL03UL09DL02UL03DL01UL03DL01UL14DL02UL07DL01UL03TDR01UR02DR02UR06DR02UR02DR01UR07DR01UR03DR01UR03DR01UR03DR02UR09DR02UR10DR01UR02DR01UR01DR01TDL01UL04DL01UL10DL02UL09DL02UL07DL01UL03DL01UL04DL01UL02DL01UL02DL04UL01DL02UL01DL03UL01DL01TDR01UR10DR02UR02DR01UR02DR01UR08DR01UR07DR02UR09DR02UR10DR01UR04DR01TDL01UL04DL02UL09DL07UL01DL02UL01DL02UL03DL01UL03DL01UL14DL02UL11TUR11DR02UR02DR01UR10DR02UR03DR01UR03DR02UR04DR01UR04DR02UR09DR02UR01DR01UR02DR01TDL01UL02DL01UL01DL02UL09DL02UL04DL01UL04DL02UL03DL02UL02DL02UL02DL02UL02DL02UL02DL02UL01DL02UL03DL01UL03DL01UL02DL01TDR01UR05DR02UR02DR03UR05DR01UR07DR02UR02DR02UR03DR02UR04DR01UR04DR02UR09DR02UR01DR04TDL01UL04DL02UL09DL02UL04DL01UL04DL02UL07DL02UL07DL01UL05DL02UL10DL01TUR11DR02UR05DR01UR07DR02UR07DR01UR05DR01UR04DR02UR09DR02UR04DR01TDL01UL03DL14UL04DL01UL05DL01UL03DL01UL03DL02UL13DL04UL01DL02UL01DL03UL01DL01TDR01UR09DR03UR13DR02UR03DR01UR03DR01UR10DR18TDL01UL04DL02UL09DL02UL10DL01UL03DL01UL01DL07UL01DL07UL01DL04UL09DL01TUR04DR02UR04DR03UR05DR01UR07DR04UR02DR04UR10DR02UR09DR03UR02DR02TDL01UL04DL02UL09DL03UL03DL01UL05DL01UL07DL02UL07DL01UL05DL05UL01DL02UL05TDR01UR09DR03UR13DR02UR07DR09UR01DR03UR09DR02UR04DR01TDL01UL04DL24UL07DL02UL13DL03UL05DL01UL03DL01TDR01UR03DR01UR05DR03UR02DR07UR01DR07UR04DR02UR09DR19TUL28DL04UL04DL02UL07DL01UL06DL02UL10TUR10DR02UR06DR01UR07DR01UR07DR02UR28TUL28DL02UL07DL01UL14DL02UL05DL01UL04T";
  char gcode [] = "URRRRDRRRRRRRRRURRRRRDRRURRRRRDRURRRRRRRRRRRRRRDRRRURRRRRTULLLDLLLLLLULLLLLLLLLLLLLDLULLDLULLDLLULLLLLDLULDLULLLLDLLLULLLTURRRDRRRURRRRRRDRURRRRRDRRURRRRRDRURRDRURRRRRRRRDRRRURRRDRRRURRTDLLLULLLLLLDLLULLLLLLLLLLLDLLLLLLLLULLLLLDLULLLLLLDLLLULLDLTDRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRURRRRRRRDRRURTULDLLULLLLLLLLDLLLLLLLLULLLLLLLLLDLLLLLLLLLLLLLLLLLULLLTDRURRDRRRURRRRRRDRURRRRRDRRURRRRRRRRRDRURRRRRRDRURRRRRRRRRDRURTULDLLULLLLLLLLDLULLLLLDLLULLLLLLLLLDLLULLLLLDLULLLLLLDLLLULLLTURRRDRRRURRRRRRDRURRRRRDRRURRRRRRRRRDRURRRRRRDRURRRRRRRRDRRURTDLLLULLLLLLLLDLULLLLLLDLULLLLLLLLLDLLULLDLULLDLULLLLLDLLLLULLDLTURRRDRRRURRRRRRDRURRRRRDRRURRRRRRRRRDRURRRRRDRRURRRRRRRRDRRURTULDLLULLLLLLLLDLLULDLLLLULLLLLLLLLLDLLULLLLLDLULLLLLLLDLLULLLTDRURRDRRURRRRRRRDRURRRRRDRRURRRRRRRRRDRURRDRRRRRURRRRRRRRDRRURTULDLLULLLLLLLLDLLLULLLLDLULLLLLLLLLDLLULLLLLLLLLLLLLDLLULLDLTDRRRRRRRRRRRRRURRRRRDRRURRRRRRRRRDRURRRRDRRRURRRRRRRRDRRURTDLLLLLLLLLLLLLLLULLLDLULLLLDLLULLLDLLULLLLLLDLULLLLLLDLLULLLTDRURRDRRURRRRRRRRRRRRRDRRURRRDRRRRRRRRRRRURDRRRRRRRRRRRRRTULDLLULLLLLLLLDLLULLLLLDLULLLLLDLULLLLDLULLLLLLDLULLLLLLDLLULLLTURRRDRRURRRDRURRDRURRRRRRDRURRRRDRURRRRRRDRURRRRDRRURRRRRRRRDRRURTULDLLULLLLLLLLDLLULLLLDLULLLLLLDLULLLLDLULLLLLLDLLLLULDLLLLLULLTURDRRURRDRRURRRRDRURRRRRRDRURRRRRRRRRRRDRURRRRDRRURRRRRRRRDRRURTULLDLULLLLLLLLDLULLLLLDLULLLLLLDLULLLLDLULLLLLLDLULLLLDLULLLLDLLTDRURRRRRDRRURRRDRURRRRRRDRURRRRDRRRURRRDRRURRRDRURDRURRRRRRRRDRURRTULLDLULLLLLLLLDLULLLLLLDLLLLLULLLLLLDLLLLLLLLLLLLLULLLLDLLTDRRURRRDRRURRRDRURRRRRRDRRURRRRRRDRRURRRRRRRRRDRURRRRRRRRDRURRTDLLLULLLLLLLDLLULLLLLLLLLDLLLLLLLLLULLLLLLLDLULLLDLLLLLLULTURRDRRRURRRRRDRURRRRRRDRRURRRRRRRDRURRRRRRRRRDRRURRRRRRDRRRRTULLLDLLULLLLLDLLULLLLLLLLLDLULLLLLLLDLLULLLLLLDLULLLLLDLLLULLTURRDRRRURRRRRDRURRRRRRDRRURRRRRRRDRURRRRRRRRRRDRRURRRDRRRURRDRTULLLLDLLLLLLULLLLLLLLLLLDLLULLLLLLDLLULLLLLLDLULLLLDLLLLULLTURRDRRRURRRRRDRURRRRRRDRRURRRRRRRDRURRRRRRRRRRRRDRRRURRRRRRTULLLLLLDLLLULLLLLLLLLLLLDLULLLLLLLDLLULLLLLLDLULLLLLDLLLLLTDRRRRRRRRRRRRRRRRURDRRURRRRRRRDRURRRRRRRRRRRRDRRRURRRRRRTULLLLLLDLLLULLLLDLLLLLLLLLLLLLLLLLLLLLLLLLLLLULLLDLLULLTURRDRRURRRRRRDRURRRDRURRRDRRURDRRRRRRRRRRRRRRRRRRRRRRRRRRRTDLLLULLLDLLLULLLLLLLLLLDLLULLLDLULDLULDLLULLLDLULLLDLULLLLLLDLLULLTURRDRRURRDRURRRDRURRRDRURRRDRRURDRURDRRRURDRRURRRRRDRURRRRDRRURRRRRRRTULLLLLLLDLLULLLLLLLLLLDLLULLLDLULLLDLLULLLDLULLLDLLULLDLULLDLLULLTDRRRRURRDRURRDRRURRRDRURRRDRRURRRDRURRRDRRURRRRRRRRRRDRRURRRRRRRTULLLLLLLDLLULLLLLLLLLLDLLULLLDLULLLDLLULLLDLULLLLLLLDLULLDLLULDLTURRDRRURRDRURRRDRURRRDRURRRDRRURRRDRURRRDRRRRRRRRRRRRRRRURRRRRRTULLLLLLDLLLULLLLDLULLLLLDLLULLLDLULDLULDLLULLLDLULLLDLLULLDLULLDLLULLTURDRRRRRRRRRRRRRRRRRRRRRURDRURRRDRRURRRRRDRURRRRDRRURRRRRRRTULLLLLLLDLLLULLLDLULLLLDLLLULLLDLLLULDLLULLLLLLLLDLULLLLLDLLLLTURDRRRURRRRRDRRURRRRRRRDRRURRRDRURRRDRRRURRRRDRURRRDRRRURRRRRRRTULLLLLLLDLLULLLLLLLLLLDLLULLLDLULLLDLLULLLLLLLLDLULLLLLDLLULLTURRDRRURRRRRRRRRRRRRRDRRURRRDRURRRRRRRRRRRRRRRRRRRRRRRRTULLLLLLLLLLLLLLLLLLLLLLDLULDLULLLLDLULLLLLLLLDLULLLLLDLLULLT";
  for (int c = 0; c < strlen(gcode); ){
    char cmd = gcode[c];
    int j = 0;
    Serial.println(cmd);

    switch (cmd)
    {
    case 'D':
      zmove(0);
      c++;
      break;
    case 'U':
      zmove(1);
      c++;
      break;
    case 'T':
      move(0,superstep_size);
      c++;
      break;
    case 'B':
      move(0,-superstep_size);
      c++;
      break;
    case 'R':
      j = 0;
      // j += atoi((char)gcode[c+1])*10 + atoi((char) gcode[c+2]);
      // c++;
      // c++;
      // c++;
      while(gcode[c] == 'R'){
        c++;
        j++;
      }
      Serial.println(j);

      move(superstep_size * j,0);
      break;

    case 'L':
      j = 0;
      // j += atoi((char) gcode[c+1])*10 + atoi((char) gcode[c+2]);
      // c++;
      // c++;
      // c++;
      while(gcode[c]=='L'){
        c++;
        j++;
      }
      Serial.println(j);
      move(-superstep_size*j,0);
      break;
    default:
      Serial.println("INVALID COMMAND");
      break;
    }
  delay(20);
  }
    Serial.print("[ ");
    Serial.print(cx);
    Serial.print("\t");
    Serial.print(cy);
    Serial.print("\t");
    Serial.print(cz);
    Serial.println (" ] ");
  delay(3000);
  autoctr += 1;

}
