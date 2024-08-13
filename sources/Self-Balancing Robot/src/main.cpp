#include <Arduino.h>

// USER INCLUDE SCOPE
#include "center_controller.hpp"


void setup() {
  Serial.begin(115200);

  while (!Serial)
  {
    delay(10);
  }

  PID_block_component = new PID_block();
  mpu_reader_component = new mpu_reader();
  motor_controller_component = new motor_controller();
  center_controller_component = new center_controller();
  display_controller_component = new display_controller();
  
  center_controller_component->run();

}

void loop() {
}