#include "test-midi.h"

void runTestMIDI() {
  if (usbMIDI.read()) {
    byte type = usbMIDI.getType();
    byte channel = usbMIDI.getChannel();
    byte data1 = usbMIDI.getData1();
    byte data2 = usbMIDI.getData2();

    Serial.print("MIDI Type: ");
    Serial.print(type);
    Serial.print("  Channel: ");
    Serial.print(channel);
    Serial.print("  Data1: ");
    Serial.print(data1);
    Serial.print("  Data2: ");
    Serial.println(data2);
  }
}
