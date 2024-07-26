#include "center_controller.hpp" 

center_controller_struct center_controller = {0,0,0};

SemaphoreHandle_t xMutex_center_controller;

void center_controller_init(){
    xMutex_center_controller = xSemaphoreCreateMutex();
}