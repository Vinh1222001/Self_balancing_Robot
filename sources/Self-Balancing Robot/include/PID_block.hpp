#pragma once
#ifndef PID_BLOCK_HPP
#define PID_BLOCK_HPP

#include <Arduino.h>
#include "center_controller.hpp"
#include "motor_controller.hpp"
#include "mpu_reader.hpp"
#include "global.hpp"

struct PID_block
{
    float filted_pitch;
    float gyro_angle_Y;
    float output;
    float error;
    float integral;
    float derivative;
};

extern struct_PID_parameters PID_block_PID_params;

extern PID_block PID_values;

extern QueueHandle_t q_PID_values;

extern TaskHandle_t PID_task_handle;

void PID_init();

void PID_run(void);

#endif
