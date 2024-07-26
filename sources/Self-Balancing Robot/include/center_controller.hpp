#pragma once
#ifndef CENTER_CONTROLLER_HPP
#define CENTER_CONTROLLER_HPP

#include <Arduino.h>
struct center_controller_struct
{
    double Kp;
    double Ki;
    double Kd;
};

extern SemaphoreHandle_t xMutex_center_controller;

extern center_controller_struct center_controller;

void center_controller_init();
#endif
