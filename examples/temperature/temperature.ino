#include <Temperature.h>


Temperature temp;


void setup(void) {
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  temp.begin(10);
  Serial.printf("ROM = %s\n", temp.address());
  Serial.printf("Chip = %s\n", temp.chip());
}

void loop(void) {
  temp.request();    
  delay(temp.delay());
  temp.read();
  Serial.printf("T=%5.2fC\n", temp.temperature());
  delay(2000);
}
