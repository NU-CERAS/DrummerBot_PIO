// === Includes ===
// Include custom header for potentiometer setup function declarations
#include "pot-setup.h"

// Include project constants and shared global variables
#include "constants.h"

// === Function: updateNeutralPositions ===
// Purpose: Read potentiometer values to dynamically adjust the neutral (resting) position of each servo
//          Also update the corresponding hit position based on servo type (Dal or Kal)
void updateNeutralPositions() {
  // Loop through each servo
  for (int i = 0; i < NUM_SERVOS; i++) {
    // Read the analog value from the potentiometer assigned to this servo
    int potValue = analogRead(PotPins[i]);

    // Map the potentiometer reading (0–1023) to the servo's low–high neutral position range
    neutPos[i] = map(potValue, 0, 1023, lowNeutPos[i], highNeutPos[i]);

    // Adjust the hit position based on servo type:
    // - Dal type (servoTypes[i] == 1): offset negatively by 5 degrees
    // - Kal type (servoTypes[i] == 0): offset positively by 5 degrees
    hitPos[i] = neutPos[i] + (servoTypes[i] == 1 ? -5 : 5);
  }
}
