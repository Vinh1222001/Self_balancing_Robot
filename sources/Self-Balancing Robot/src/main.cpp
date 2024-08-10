#include <Arduino.h>

// USER INCLUDE SCOPE
#include "center_controller.hpp"


void setup() {
  Serial.begin(115200);

  while (!Serial)
  {
    delay(10);
  }
  
  center_controller_component.run();

}

void loop() {
}