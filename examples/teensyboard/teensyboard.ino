#include <TeensyBoard.h>


void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  Serial.printf("Board:  %s\n", teensyBoard());
  Serial.printf("Serial: %s\n", teensySN());
  Serial.printf("MAC:    %s\n", teensyMAC());
}


void loop() {
}
