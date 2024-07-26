#pragma once
#ifndef BUTTON_READER_HPP
#define BUTTON_READER_HPP

#include <Arduino.h>

#define DEBOUNCE_THRESHOLE 220

#define BTN_OK      13
#define BTN_ESC     27
#define BTN_DOWN    33
#define BTN_UP      32

struct Button {
    const uint8_t PIN;
    // uint32_t numberKeyPresses;
    volatile bool buttonPressed;
    volatile unsigned long lastDebounceTime;
    volatile int counter;
};

void init_ctrl_btn(Button btn);

void IRAM_ATTR handleButtonPress(void* arg);

void attach_btn_interrupt(Button* btn);

void button_init(void);

extern Button btn_OK; 
extern Button btn_ESC; 
extern Button btn_UP; 
extern Button btn_DOWN; 


#endif