#include "test-pot.h"
extern int neutPos[6];
extern int lowNeutPos[6];
extern int highNeutPos[6];
extern const int PotPins[6];

void updateNeutralPositions(){
    for (int i = 0; i < 6; i++) {
        int potValue = analogRead(PotPins[i]);      // Read potentiometer
        neutPos[i] = map(potValue, 0, 1023, lowNeutPos[i], highNeutPos[i]); // Map to servo neutral range (adjust range as needed)    
    }
}

void runTestPOT(){
    updateNeutralPositions();
    Serial.print("Updated neutral positions: ");
    for (int i = 0; i < 6; i++) {
        Serial.print("Servo ");
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.print(neutPos[i]);
        if (i < 5) {
            Serial.print(", ");
        }
    }
    printf("\n");
    delay(100);
}

