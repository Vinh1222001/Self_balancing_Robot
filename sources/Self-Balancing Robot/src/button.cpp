#include "button.hpp"

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