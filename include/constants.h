#pragma once
#include <Arduino.h>
#include <Servo.h>
#include <AccelStepper.h>

// Constants
const int NUM_SERVOS = 6;

extern const int PotPins[NUM_SERVOS];
extern const int potSetupBtnPin;
extern const int KK1;
extern const int KK2;
extern const int servoPins[NUM_SERVOS];
extern const int BND1DIR;
extern const int BND1STP;
extern const int BND2DIR;
extern const int BND2STP;
extern const int TWIDIR;
extern const int TWISTP;

// MIDI notes
extern const int MKK;
extern const int MD1;
extern const int MD2;
extern const int MD3;
extern const int MD4;
extern const int MD5;
extern const int MD6;
extern const int MTWI; 
extern const int MBND;

// Servo types
extern const int servoTypes[NUM_SERVOS];

// Servo hit settings
extern const int maxVelDal;
extern const int maxVelKal;
extern const long interval;

// Hat Pedal Servo
extern const int HATP_SERVO_PIN;
extern const int HATP_MIDI_NOTE;
extern const int hatPedActiveAngle;
extern const int hatPedRestAngle;
extern Servo hatPedServo;

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

extern const int bendSpeed;
extern const int bendAcc;
extern const int bendPwidth;
extern AccelStepper bendStepper1;
extern AccelStepper bendStepper2;

