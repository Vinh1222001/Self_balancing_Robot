#pragma once
#ifndef CENTER_CONTROLLER_HPP
#define CENTER_CONTROLLER_HPP

#include <Arduino.h>
#include "EEPROM.h"
#include "mpu_reader.hpp"
#include "motor_controller.hpp"
#include "PID_block.hpp"

struct struct_PID_parameters
{
    float Kp;
    float Ki;
    float Kd;
};

extern struct_PID_parameters center_controller_PID_params;

extern SemaphoreHandle_t xMutex_PID_parameters;

extern const int eeprom_Kp_address;
extern const int eeprom_Ki_address;
extern const int eeprom_Kd_address;

struct struct_angle_values{
    float pitch;
    float gyroY;
};


extern QueueHandle_t q_angle_values;

extern struct_angle_values angle_values;

extern bool start_robot_flag;

extern SemaphoreHandle_t xMutex_start_robot_flag;

void center_controller_init();

void center_controller_run();
#endif
