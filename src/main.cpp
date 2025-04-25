#include <Arduino.h>
#include "test-midi.h"  // Notice the updated filename!

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Starting USB MIDI Reader...");
}

void loop() {
  runTestMIDI();  // call your function
}
