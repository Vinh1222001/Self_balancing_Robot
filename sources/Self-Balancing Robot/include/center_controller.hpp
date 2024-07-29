#pragma once
#ifndef CENTER_CONTROLLER_HPP
#define CENTER_CONTROLLER_HPP

#include <Arduino.h>
#include "mpu_reader.hpp"

struct struct_PID_parameters
{
    double Kp;
    double Ki;
    double Kd;
};

struct struct_angle_values{
    float pitch;
    float yaw;
};

extern SemaphoreHandle_t xMutex_PID_parameters;

extern struct_PID_parameters center_controller;

extern QueueHandle_t q_angle_values;

extern struct_angle_values angles_value;

void center_controller_init();

void center_controller_run();
#endif
