#pragma once
#ifndef PID_BLOCK_HPP
#define PID_BLOCK_HPP

#include <Arduino.h>
#include "center_controller.hpp"
#include "motor_controller.hpp"
#include "mpu_reader.hpp"
#include "global.hpp"

class PID_block
{
private:

    const char* TAG = "PID BLOCK";
    const int delay_time;

    unsigned long prev_time;
    const uint8_t priority;

    const float alpha;

    TaskHandle_t PID_task_handle;

    struct{

        QueueHandle_t queue;
        struct_PID_block values;
        
    } PID_values;

    struct_PID_parameters PID_params;

    void PID_compute();

    static void PID_compute_wrapper(void* arg);

public:
    PID_block(int delay_time = 10, uint8_t priority = 7, float alpha = 0.9934);
    ~PID_block();

    QueueHandle_t get_PID_values_queue();
    TaskHandle_t get_task_handle();
    void set_PID_parameters(struct_PID_parameters params);
    void run();

    void set_prev_time();
};

extern PID_block* PID_block_component;


#endif
