// === Includes ===
#include "stepper-control.h"
#include "constants.h"
#include <AccelStepper.h>

void setupSteppers(){
    twistStepper.setMaxSpeed(twistSpeed);
    twistStepper.setAcceleration(twistAcc);
    twistStepper.setMinPulseWidth(twistPwidth); 
}

void runSteppers(){
    twistStepper.run();
    if (twistStepper.distanceToGo() != 0) {
        Serial.println("Stepper moving to: " + String(twistStepper.targetPosition()));
      }
      
}
