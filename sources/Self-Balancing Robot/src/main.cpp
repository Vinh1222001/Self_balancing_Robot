#include <Arduino.h>

// USER INCLUDE SCOPE
#include "pins_define.hpp"
#include "display_controller.hpp"
#include "global.hpp"

#include "mpu_reader.hpp"
#include "motor_controller.hpp"
#include "center_controller.hpp"

void setup() {
  Serial.begin(115200);

  while (!Serial)
  {
    delay(10);
  }
  

  init_display(SCREEN_WIDTH, SCREEN_HEIGHT);

  button_init();

  center_controller_init();
  
  mpu_reader_init();

  mpu_reader_run();

  center_controller_run();

  menu_run();


}

void loop() {

}