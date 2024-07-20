#include <Arduino.h>

// USER INCLUDE SCOPE
#include "pins_define.hpp"
#include "oled_display.hpp"
#include "button.hpp"
#include "global.hpp"



void setup() {
  Serial.begin(115200);

  // init_ctrl_btn(btn_OK);
  // init_ctrl_btn(btn_ESC);
  // init_ctrl_btn(btn_UP);
  // init_ctrl_btn(btn_DOWN);

  // attach_btn_interrupt(&btn_OK);
  // attach_btn_interrupt(&btn_ESC);
  // attach_btn_interrupt(&btn_UP);
  // attach_btn_interrupt(&btn_DOWN);

  // xMutex_menu_curr_opt = xSemaphoreCreateMutex();

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally

  init_display(SCREEN_WIDTH, SCREEN_HEIGHT);

  menu_start();

}

void loop() {

}