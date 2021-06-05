#include <PushButtons.h>

int ButtonPin = 24;

PushButtons buttons;


void onpress(int id) {
  Serial.printf("\nPressed button %d\n", id);
}


void onrelease(int id) {
  Serial.printf("Released button %d\n", id);
}


void setup() {
  Serial.begin(9600);
  buttons.add(ButtonPin, INPUT_PULLUP, onpress, onrelease);
}


void loop() {
  buttons.update();
}
