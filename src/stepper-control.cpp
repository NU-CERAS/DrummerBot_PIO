// === Includes ===
#include "stepper-control.h"
#include "constants.h"
#include <AccelStepper.h>

void setupSteppers(){
    twistStepper.setMaxSpeed(twistSpeed);
    twistStepper.setAcceleration(twistAcc);
    twistStepper.setMinPulseWidth(twistPwidth); 
    
    bendStepper1.setMaxSpeed(bendSpeed);
    bendStepper1.setAcceleration(bendAcc);
    bendStepper1.setMinPulseWidth(bendPwidth); 
    
    bendStepper2.setMaxSpeed(bendSpeed);
    bendStepper2.setAcceleration(bendAcc);
    bendStepper2.setMinPulseWidth(bendPwidth); 
}

void runSteppers(){
    twistStepper.run();
    bendStepper1.run();
    bendStepper2.run();

    //Serial
    if (twistStepper.distanceToGo() != 0) {
        Serial.println("Twist Stepper moving to: " + String(twistStepper.targetPosition()));
    }
    
    if (bendStepper1.distanceToGo() != 0) {
        Serial.println("Bend stepper moving to: " + String(bendStepper1.targetPosition()));
    }   
}
