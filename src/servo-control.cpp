// === Includes ===
#include "servo-control.h"
#include "constants.h"

// === Function: adjustedVelocityControlByte ===
// Purpose: Limit the incoming velocity control byte to a usable range (40 to 120)
// This ensures that very low or very high velocity values do not cause extreme servo behavior.
int adjustedVelocityControlByte(int velocityControlByte) {
  return constrain(velocityControlByte, 40, 120);
}

// === Function: velocityControl ===
// Purpose: Calculate the servo target position based on the adjusted velocity value and servo type
// Different formulas are used for Dal and Kal servos to control motion direction properly.
int velocityControl(int changedVelocityControlByte, int servoIndex) {
  if (servoTypes[servoIndex] == 1) { // Dal servo (moves toward smaller angle when hitting)
    return neutPos[servoIndex] - ((changedVelocityControlByte - 40) * (neutPos[servoIndex] - maxVelDal) / 80);
  } else { // Kal servo (moves toward larger angle when hitting)
    return neutPos[servoIndex] + ((changedVelocityControlByte - 40) * (neutPos[servoIndex] - maxVelKal) / 80);
  }
}

// === Function: initializeServos ===
// Purpose: Attach each Servo object to its assigned control pin and move it to its neutral starting position
void initializeServos() {
  for (int i = 0; i < NUM_SERVOS; i++) {
    servos[i].attach(servoPins[i]);    // Attach the servo to its PWM pin
    servos[i].write(neutPos[i]);        // Move the servo to its neutral position

    hatPedServo.attach(HATP_SERVO_PIN);
    hatPedServo.write(hatPedRestAngle); // Start in resting position  
  }
}

// === Function: updateServoHits ===
// Purpose: Manage servo state transitions based on elapsed time
// If a servo has been in action for longer than the defined interval, move it to its hit position and deactivate its action flag
void updateServoHits(unsigned long currentMillis) {
  for (int i = 0; i < NUM_SERVOS; i++) {
    // Check if servo is active and enough time has passed since its activation
    if (servoAction[i] && (currentMillis - previousMillis[i] >= interval)) {
      servos[i].write(hitPos[i]);      // Move the servo to its designated hit position
      servoAction[i] = false;          // Mark the servo as inactive after completing its hit
    }
  }
}
