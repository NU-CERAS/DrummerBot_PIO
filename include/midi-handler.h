#pragma once
#include <Arduino.h>

void readAndProcessMIDI();
void handleKickDrum(byte type, byte velocity);
void handleServoMIDI(byte type, byte note, byte velocity);
void handleStepperMIDI(byte type, byte note, byte velocity);