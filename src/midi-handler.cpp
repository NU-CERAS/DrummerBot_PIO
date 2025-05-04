// === Includes ===
// Include project-specific headers
#include "midi-handler.h"   // Declarations for MIDI message processing
#include "servo-control.h"  // Declarations for servo velocity control functions
#include "constants.h"      // Shared constants like pin numbers, MIDI note mappings, and global variables
#include <AccelStepper.h>
// === Function: handleKickMIDI ===
// Purpose: Control the kick drum output pins based on incoming MIDI messages
void handleKickMIDI(byte type, byte velocity) {
  // If a Note On message is received with nonzero velocity, activate kick drum outputs
  if (type == usbMIDI.NoteOn && velocity > 0) {
    digitalWrite(KK1, HIGH);
    digitalWrite(KK2, HIGH);
  }
  // If a Note Off message or a Note On with zero velocity is received, deactivate kick drum outputs
  else if (type == usbMIDI.NoteOff || (type == usbMIDI.NoteOn && velocity == 0)) {
    digitalWrite(KK1, LOW);
    digitalWrite(KK2, LOW);
  }
}

// === Function: handleServoMIDI ===
// Purpose: Control servo movements based on incoming MIDI messages
void handleServoMIDI(byte type, byte note, byte velocity) {
  // Map the incoming MIDI note to the corresponding servo index
  int servoIndex = -1;
  if (note == MD1) servoIndex = 0;
  else if (note == MD2) servoIndex = 1;
  else if (note == MD3) servoIndex = 2;
  else if (note == MD4) servoIndex = 3;
  else if (note == MD5) servoIndex = 4;
  else if (note == MD6) servoIndex = 5;
  else return;  // Invalid note

  unsigned long currentMillis = millis();  // Capture the current time for timing calculations

  // Handle Note On with nonzero velocity (servo hit action)
  if (type == usbMIDI.NoteOn && velocity > 0) {
      // Adjust velocity using helper functions and command servo to new position
      servoValues[servoIndex] = velocityControl(adjustedVelocityControlByte(velocity), servoIndex);
      servos[servoIndex].write(servoValues[servoIndex]);

      // Record the current time for timing when to reset
      previousMillis[servoIndex] = currentMillis;

      // Mark this servo as active (in hitting state)
      servoAction[servoIndex] = true;
  }
  // Handle Note Off or Note On with zero velocity when the servo was actively hitting
  else if ((type == usbMIDI.NoteOff || (type == usbMIDI.NoteOn && velocity == 0)) && servoAction[servoIndex]) {
      servoAction[servoIndex] = false;  // Mark servo as no longer active
      servos[servoIndex].write(hitPos[servoIndex]);  // Move servo to its designated hit position
  }
  // Handle Note Off or Note On with zero velocity if the servo was already idle
  else if ((type == usbMIDI.NoteOff || (type == usbMIDI.NoteOn && velocity == 0)) && !servoAction[servoIndex]) {
      servos[servoIndex].write(neutPos[servoIndex]);  // Return servo to its neutral resting position
  }
}


// === Function: handleHatPedServoMIDI
// Purpose: Control the hat pedal servo based on incoming MIDI messages
void handleHatPedServoMIDI(byte type, byte velocity) {
  if (type == usbMIDI.NoteOn) {
    hatPedServo.write(hatPedActiveAngle);
  } else if (type == usbMIDI.NoteOff) {
    hatPedServo.write(hatPedRestAngle);
  }
}


// === Function: handleStepperMIDI
// Purpose: Control stepper motor movements based on incoming MIDI messages

void handleStepperMIDI(byte type, byte note, byte velocity) {
  if (type == usbMIDI.NoteOn) {
    if(note == MTWI){
      int targetPos = map(velocity, 0, 127, -twistPwidth, twistPwidth);
      twistStepper.moveTo(targetPos); // absolute control of pos
      Serial.print("Moving twist stepper to absolute position: ");
      Serial.println(targetPos);
    }
    else if(note == MBND){
      int targetPos = map(velocity, 0, 127, -bendPwidth, bendPwidth);
      bendStepper1.moveTo(targetPos);
      bendStepper2.moveTo(-targetPos); //negative of targetPos so the stepper motors move together
      Serial.print("Moving bend stepper to absolute position: ");
      Serial.println(targetPos);
    }
  }
}


// === Function: readAndProcessMIDI ===
// Purpose: Continuously read available MIDI messages and dispatch them to the correct handler (kick or servo)
void readAndProcessMIDI() {
  while (usbMIDI.read()) { // Process all available incoming MIDI messages
    byte type = usbMIDI.getType();    // Get MIDI message type (Note On, Note Off, etc.)
    byte note = usbMIDI.getData1();   // Get MIDI note number (which drum or servo to activate)
    byte velocity = usbMIDI.getData2(); // Get MIDI velocity (how hard to hit)

    // If the note corresponds to kick drum, handle it
    if (note == MKK) {
      handleKickMIDI(type, velocity);
    }
    // If the note corresponds to one of the servos, handle it
    else if (note == HATP_MIDI_NOTE) {
      handleHatPedServoMIDI(type, velocity);
    }    
    else if (
      (note == MD1 || 
       note == MD2 || 
       note == MD3 || 
       note == MD4 || 
       note == MD5 || 
       note == MD6) &&
      note != HATP_MIDI_NOTE
  ) {
      handleServoMIDI(type, note, velocity);
  }
     
    // If the note corresponds to the stepper motors, handle it
    else if(note == MTWI || note == MBND){
      handleStepperMIDI(type, note, velocity);
    }
  }
}

void printMIDIMessage() {
  while (usbMIDI.read()) {
    byte type = usbMIDI.getType();
    byte channel = usbMIDI.getChannel();
    byte data1 = usbMIDI.getData1();
    byte data2 = usbMIDI.getData2();

    Serial.print("[MIDI] Type: ");
    Serial.print(type);
    Serial.print(" | Channel: ");
    Serial.print(channel);
    Serial.print(" | Data1 (Note): ");
    Serial.print(data1);
    Serial.print(" | Data2 (Velocity/Value): ");
    Serial.println(data2);
  }
}






