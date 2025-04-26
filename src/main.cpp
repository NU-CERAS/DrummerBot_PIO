#include "test-midi.h"  // Your custom file
#include "pot-setup.h"
#include <Arduino.h>
#include <Servo.h>

// Define new neutral positions for each servo, their range, and potentiometer adjustment pins
int neutPos[6] = {85, 87, 80, 81, 80, 101};
int lowNeutPos[6] = {65, 67, 60, 61, 60, 81};
int highNeutPos[6] = {105, 107, 100, 101, 100, 121};

int PotPins[6] = {14, 15, 16, 17, 18, 19};
const int buttonPin = 13;
int buttonState = 0;
// Define hit positions (Kal servos: +5, Dal servos: -5)
int hitPos[6] = {80, 82, 75, 76, 75, 106};

const int maxVelDal = 125;
const int maxVelKal = 160;

// Define MIDI values for each servo
const int MKK = 36;
const int MD1 = 37;
const int MD2 = 38;
const int MD3 = 39;
const int MD4 = 40;
const int MD5 = 41;
const int MD6 = 42;

// Define kick drum pins 
const int KK1 = 20;
const int KK2 = 21;

// Define servo pins
const int servoPins[] = {2, 3, 4, 5, 6, 7};
Servo servos[6];

// Define variables to store the current position and action state for each servo
int servoValues[6] = {0, 0, 0, 0, 0, 0};
unsigned long previousMillis[6] = {0, 0, 0, 0, 0, 0};  // Store timing for each servo
const long interval = 20;                              // Interval for servo movement in milliseconds
bool servoAction[6] = {false, false, false, false, false, false};  // Track action states for each servo

// Define servo types: 0 for Kal, 1 for Dal
const int servoTypes[6] = {1, 1, 1, 1, 1, 0};  // MD2 and MD5 are Dal, others are Kal

// Helper Functions
int adjustedVelocityControlByte(int velocityControlByte) {
  return constrain(velocityControlByte, 40, 120);
}

int velocityControl(int changedVelocityControlByte, int servoIndex) {
  if (servoTypes[servoIndex] == 1) {  // Dal servos
    return neutPos[servoIndex] - ((changedVelocityControlByte - 40) * (neutPos[servoIndex] - maxVelDal) / 80);
  } else {                            // Kal servos
    return neutPos[servoIndex] + ((changedVelocityControlByte - 40) * (neutPos[servoIndex] - maxVelKal) / 80);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(KK1, OUTPUT);
  pinMode(KK2, OUTPUT);

  // Attach each servo to its corresponding pin
  for (int i = 0; i < 6; i++) {
    servos[i].attach(servoPins[i]);
    servos[i].write(neutPos[i]); // Start servos at neutral position
  }
}

void loop() {
  buttonState = digitalRead(buttonPin);
  if(buttonState == LOW){
    updateNeutralPositions();
    for (int i = 0; i < 6; i++) {
      servos[i].write(neutPos[i]); // Start servos at neutral position
    }
    delay(10);
  }

  unsigned long currentMillis = millis();
  bool note_over = false;

  // Read and process all incoming MIDI messages
  while (usbMIDI.read()) {
    byte type = usbMIDI.getType();
    byte note = usbMIDI.getData1();
    byte velocity = usbMIDI.getData2();

    if (note == MKK) {  // Kick drum control
      if (type == usbMIDI.NoteOn && velocity > 0) {
        digitalWrite(KK1, HIGH);
        digitalWrite(KK2, HIGH);
      } 
      else if (type == usbMIDI.NoteOff || (type == usbMIDI.NoteOn && velocity == 0)) {
        digitalWrite(KK1, LOW);
        digitalWrite(KK2, LOW);
      }
    }
    else if (note >= MD1 && note <= MD6) {  // Servo control
      int servoIndex = note - MD1;

      if (type == usbMIDI.NoteOn && velocity > 0) {
        servoValues[servoIndex] = velocityControl(adjustedVelocityControlByte(velocity), servoIndex);
        servos[servoIndex].write(servoValues[servoIndex]);
        previousMillis[servoIndex] = currentMillis;
        servoAction[servoIndex] = true;
      } 
      else if ((type == usbMIDI.NoteOff || (type == usbMIDI.NoteOn && velocity == 0)) && servoAction[servoIndex]) {
        note_over = true;
      } 
      else if ((type == usbMIDI.NoteOff || (type == usbMIDI.NoteOn && velocity == 0)) && !servoAction[servoIndex]) {
        servoValues[servoIndex] = neutPos[servoIndex];
        servos[servoIndex].write(servoValues[servoIndex]);
      }

      // Debug print
      Serial.print("MIDI Message: ");
      Serial.print("Type ");
      Serial.print(type);
      Serial.print(" Note ");
      Serial.print(note);
      Serial.print(" Velocity ");
      Serial.println(velocity);
    }
  }

  // Handle servo hit and reset timing
  for (int i = 0; i < 6; i++) {
    if (servoAction[i] && ((currentMillis - previousMillis[i] >= interval) || note_over)) {
      servos[i].write(hitPos[i]);
      servoAction[i] = false;
      note_over = false;
    }
  }
}
