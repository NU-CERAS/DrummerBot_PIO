// === Includes ===
// Include Arduino core library for basic microcontroller functions (pin control, Serial, etc.)
#include <Arduino.h>

// Include custom header files for project-specific constants and functions
#include "constants.h"        // Pin numbers, MIDI notes, servo types, and shared global variables
#include "servo-control.h"    // Functions related to initializing and updating servos
#include "midi-handler.h"     // Functions for reading and processing incoming MIDI messages
#include "pot-setup.h"        // Functions for reading potentiometers and adjusting neutral servo positions
#include "stepper-control.h"  // Functions for controlling stepper motors

int forceValue = 0;
bool forceToggle = false;

// === Arduino Setup Function ===
void setup() {
  Serial.begin(115200);           // Start Serial communication at 115200 baud for debugging in Serial Monitor

  // Set button for pot setup as internal pullup
  pinMode(potSetupBtnPin, INPUT_PULLUP); 

  // Set kick drum pins to digital output
  pinMode(KK1, OUTPUT);           
  pinMode(KK2, OUTPUT);            
  
  initializeServos();               // Attach all servos to their pins and move them to neutral starting positions
  setupSteppers();
  
}

// === Arduino Main Loop Function ===
void loop() {
  potSetupBtnState = digitalRead(potSetupBtnPin); // Read the current state of the button (LOW if pressed)

  // If button is pressed, update neutral positions based on potentiometer values
  if (potSetupBtnState == LOW) {
    updateNeutralPositions();     // Read potentiometer values and adjust servo neutral positions
    initializeServos();           // Re-initialize servos to apply the new neutral positions
    delay(10);                    // Short delay for hardware debounce and to allow servos to update
  }

  unsigned long currentMillis = millis(); // Get the current time in milliseconds since program start

  forceValue = analogRead(delayPin);
  // Serial.print(midiSignalTime);
  // Serial.print(" ");
  // Serial.println(currentMillis);
  if(forceValue > 60 && !forceToggle){
    Serial.println(currentMillis - midiSignalTime);
    delay(500);
    forceToggle = true;
  }
  else if(forceValue < 20 && forceToggle){
    forceToggle = false;
  }

  readAndProcessMIDI();            // Check for and process any incoming MIDI messages
  updateServoHits(currentMillis);  // Handle servo actions (e.g., returning to neutral) based on timing
  runSteppers();
}
