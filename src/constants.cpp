#include "constants.h"
#include <Servo.h>
#include <AccelStepper.h>

// Pin mappings
const int PotPins[NUM_SERVOS] = {14, 15, 16, 17, 18, 19};
const int potSetupBtnPin = 13;
const int KK1 = 20;
const int KK2 = 21;
const int servoPins[NUM_SERVOS] = {2, 3, 4, 5, 6, 7};
const int BND1DIR = 33;
const int BND1STP = 34;
const int BND2DIR = 35;
const int BND2STP = 36;
const int TWIDIR = 37;
const int TWISTP = 38;
const int HATP_SERVO_PIN = 8;

// MIDI notes mapping
const int MKK = 36;
const int MD1 = 37;
const int MD2 = 38;
const int MD3 = 39;
const int MD4 = 40;
const int MD5 = 41;
const int MD6 = 42;
const int MTWI = 48;
const int MBND = 49;
const int HATP_MIDI_NOTE = 43;

// Servo types
const int servoTypes[NUM_SERVOS] = {1, 1, 1, 1, 1, 0};

// Servo hit settings
// Dal servos (type 1) move towards smaller angles when hitting
// Kal servos (type 0) move towards larger angles when hitting
const int maxVelDal = 125;
const int maxVelKal = 160;

const int hatPedActiveAngle = 45;
const int hatPedRestAngle = 90;

// 	Defines the minimum time delay between a servo's initial hit action and its follow-up action (moving to the final hit position)
const long interval = 50;

// Servo positions
// These arrays define the neutral, hit, and low/high neutral positions for each servo
int hitPos[NUM_SERVOS] = {80, 82, 75, 76, 75, 106};
int neutPos[NUM_SERVOS] = {85, 87, 80, 81, 80, 101};
int lowNeutPos[NUM_SERVOS] = {65, 67, 60, 61, 60, 81};
int highNeutPos[NUM_SERVOS] = {105, 107, 100, 101, 100, 121};

// Stores the target position each servo should move to based on incoming MIDI velocity
// initialized to 0 for all servos
int servoValues[NUM_SERVOS] = {0};

// Stores the timestamp (in milliseconds) of when each servo last performed an action (like a hit)
unsigned long previousMillis[NUM_SERVOS] = {0};

// 	Tracks whether each servo is currently in an active "hitting" state
bool servoAction[NUM_SERVOS] = {false};

// Servo objects for controlling each servo motor
Servo servos[NUM_SERVOS];
Servo hatPedServo;

// Tracks state of the pot setting button
int potSetupBtnState = 0;

// Stepper Constants
AccelStepper twistStepper(AccelStepper::DRIVER, TWISTP, TWIDIR);   
const int twistSpeed = 500;                // Steps per second
const int twistAcc = 400;          // Acceleration
const int twistPwidth = 100;        // Minimum pulse width in microseconds

AccelStepper bendStepper1(AccelStepper::DRIVER, BND1STP, BND1DIR);
AccelStepper bendStepper2(AccelStepper::DRIVER, BND2STP, BND2DIR);
const int bendSpeed = 50;   // Steps per second
const int bendAcc = 40;    // Acceleration
const int bendPwidth = 100;    // Minimum pulse width in microseconds
