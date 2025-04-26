#include "test-pot.h"

int newNeutPos[6] = {85, 87, 80, 81, 80, 101};
const int newPotPins[6] = {14, 15, 16, 17, 18, 19};

void updateNeutralPositions(){
    for (int i = 0; i < 6; i++) {
        int potValue = analogRead(newPotPins[i]);      // Read potentiometer
        newNeutPos[i] = map(potValue, 0, 1023, newNeutPos[i] - 20, newNeutPos[i] + 20); // Map to servo neutral range (adjust range as needed)    
    }
}

void runTestPOT(){
    unsigned long lastNeutralUpdate = 0;
    const unsigned long neutralUpdateInterval = 1000;
    unsigned long currentMillis = millis();
    if (currentMillis - lastNeutralUpdate >= neutralUpdateInterval) {
        updateNeutralPositions();
        Serial.print("Updated neutral positions: ");
        for (int i = 0; i < 6; i++) {
            Serial.print("Servo ");
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(newNeutPos[i]);
            if (i < 5) {
            Serial.print(", ");
                lastNeutralUpdate = currentMillis;
            }
        }   
    }
}
