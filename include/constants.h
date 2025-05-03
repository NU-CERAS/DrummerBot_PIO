#pragma once
#include <Arduino.h>
#include <Servo.h>
#include <AccelStepper.h>

// Constants
const int NUM_SERVOS = 7; // changed to 7

extern const int PotPins[NUM_SERVOS];
extern const int potSetupBtnPin;
extern const int KK1;
extern const int KK2;
extern const int servoPins[NUM_SERVOS];
extern const int STP1STP;
extern const int STP1DIR;

// MIDI notes
extern const int MKK;
extern const int MD1;
extern const int MD2;
extern const int MD3;
extern const int MD4;
extern const int MD5;
extern const int MD6;
extern const int MD7; // added
extern const int STP1; 

// Servo types
extern const int servoTypes[NUM_SERVOS];

// Servo hit settings
extern const int maxVelDal;
extern const int maxVelKal;
extern const long interval;

extern const int onPosTal;
extern const int offPosTal;

// Globals (mutable)
extern int hitPos[NUM_SERVOS];
extern int neutPos[NUM_SERVOS];
extern int lowNeutPos[NUM_SERVOS];
extern int highNeutPos[NUM_SERVOS];
extern int servoValues[NUM_SERVOS];
extern unsigned long previousMillis[NUM_SERVOS];
extern bool servoAction[NUM_SERVOS];
extern int potSetupBtnState;

// Servo objects
extern Servo servos[NUM_SERVOS];

//Stepper Constants
extern const int twistSpeed;
extern const int twistAcc;
extern const int twistPwidth;
extern AccelStepper twistStepper; 

