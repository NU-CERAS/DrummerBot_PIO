#pragma once

void initializeServos();
int adjustedVelocityControlByte(int velocityControlByte);
int velocityControl(int changedVelocityControlByte, int servoIndex);
void updateServoHits(unsigned long currentMillis);
