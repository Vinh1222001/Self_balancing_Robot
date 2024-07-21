#include <Arduino.h>

// USER INCLUDE SCOPE
#include "pins_define.hpp"
#include "display_controller.hpp"
#include "global.hpp"

#include "mpu_reader.hpp"


void setup() {
  Serial.begin(115200);

  while (!Serial)
  {
    delay(10);
  }
  

  // init_display(SCREEN_WIDTH, SCREEN_HEIGHT);

  // menu_start();

  init_mpu_reader();

  xTaskCreatePinnedToCore(mpu_reading, "mpu_reading", 2048, nullptr, 5,nullptr, 0);

}

void loop() {

}