#include <Arduino.h>

// USER INCLUDE SCOPE
#include "pins_define.hpp"
#include "display_controller.hpp"
#include "global.hpp"

#include "mpu_reader.hpp"
#include "motor_controller.hpp"
#include "center_controller.hpp"
#include "PID_block.hpp"
#include "button_reader.hpp"

void setup() {
  Serial.begin(115200);

  while (!Serial)
  {
    delay(10);
  }
  
  button_init();

  center_controller_init();

  init_display(SCREEN_WIDTH, SCREEN_HEIGHT);
  
  mpu_reader_init();

  motor_controller_init();

  PID_init();

  mpu_reader_run();

  motor_controller_run();
  
  PID_run();

  menu_run();

  center_controller_run();

}

void loop() {
}