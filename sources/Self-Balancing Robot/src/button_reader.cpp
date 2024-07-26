#include "button_reader.hpp"

Button btn_OK = {BTN_OK, false, 0, 0};
Button btn_ESC = {BTN_ESC, false, 0, 0};
Button btn_UP = {BTN_UP, false, 0, 0};
Button btn_DOWN = {BTN_DOWN, false, 0, 0};


void init_ctrl_btn(Button btn){

    pinMode(btn.PIN, INPUT_PULLUP);
}

portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR handleButtonPress(void* arg) {
  portENTER_CRITICAL_ISR(&mux);
  
  unsigned long currentTime = millis();

  Button* temp = static_cast<Button*>(arg);

  if(currentTime != temp->lastDebounceTime){

    if ((currentTime - temp->lastDebounceTime) > DEBOUNCE_THRESHOLE) {
      temp->buttonPressed = true;
      temp->lastDebounceTime = currentTime;
    }
  }

  portEXIT_CRITICAL_ISR(&mux);
}

void attach_btn_interrupt(Button* btn){

    attachInterruptArg(btn->PIN, handleButtonPress, btn, FALLING);
}

void button_init(void){
  // Initialize buttons and attach them to Interrupt
  init_ctrl_btn(btn_OK);
  init_ctrl_btn(btn_ESC);
  init_ctrl_btn(btn_UP);
  init_ctrl_btn(btn_DOWN);

  attach_btn_interrupt(&btn_OK);
  attach_btn_interrupt(&btn_ESC);
  attach_btn_interrupt(&btn_UP);
  attach_btn_interrupt(&btn_DOWN);
}