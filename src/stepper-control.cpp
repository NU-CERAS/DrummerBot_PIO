// === Includes ===
#include "stepper-control.h"
#include "constants.h"
#include <AccelStepper.h>

void setupSteppers(){
    twistStepper.setMaxSpeed(speed);
    twistStepper.setAcceleration(acceleration);
}

void runSteppers(){
    twistStepper.run();
    Serial.println("Current Stepper Pos: " + twistStepper.currentPosition());
}
