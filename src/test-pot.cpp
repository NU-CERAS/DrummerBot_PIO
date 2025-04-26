#include "test-pot.h"

int newNeutPos[6] = {85, 87, 80, 81, 80, 101};
int lowNeutPos[6] = {65, 67, 60, 61, 60, 81};
int highNeutPos[6] = {105, 107, 100, 101, 100, 121};
const int newPotPins[6] = {14, 15, 16, 17, 18, 19};

void updateNeutralPositions(){
    for (int i = 0; i < 6; i++) {
        int potValue = analogRead(newPotPins[i]);      // Read potentiometer
        newNeutPos[i] = map(potValue, 0, 1023, lowNeutPos[i], highNeutPos[i]); // Map to servo neutral range (adjust range as needed)    
    }
}

void runTestPOT(){
    updateNeutralPositions();
    Serial.print("Updated neutral positions: ");
    for (int i = 0; i < 6; i++) {
        Serial.print("Servo ");
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.print(newNeutPos[i]);
        if (i < 5) {
            Serial.print(", ");
        }
    }
    printf("\n");
    delay(100);
}

