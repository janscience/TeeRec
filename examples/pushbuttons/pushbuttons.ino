#include <PushButtons.h>

int ButtonPin = 30;

PushButtons buttons;

void onpress(int id) {
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.printf("\nPressed button %d\n", id);
}


void onrelease(int id) {
  digitalWrite(LED_BUILTIN, LOW);
  Serial.printf("Released button %d\n", id);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  Serial.println("PushButtons test");
  digitalWrite(LED_BUILTIN, LOW);
  buttons.add(ButtonPin, INPUT_PULLUP, onpress, onrelease);
}


void loop() {
  buttons.update();
}
