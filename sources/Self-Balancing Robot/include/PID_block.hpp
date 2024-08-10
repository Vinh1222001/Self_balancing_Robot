#pragma once
#ifndef PID_BLOCK_HPP
#define PID_BLOCK_HPP

#include <Arduino.h>
#include "center_controller.hpp"
#include "motor_controller.hpp"
#include "mpu_reader.hpp"
#include "global.hpp"

/*
extern struct_PID_parameters PID_block_PID_params;

extern PID_block PID_values;

extern QueueHandle_t q_PID_values;

extern TaskHandle_t PID_task_handle;

void PID_init();

void PID_run(void);
*/

class PID_block
{
private:

    const char* TAG = "PID BLOCK";
    const int delay_time;
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
    PID_block(int delay_time = 10, uint8_t priority = 0, float alpha = 0.9934);
    ~PID_block();

    QueueHandle_t get_PID_values_queue();
    TaskHandle_t get_task_handle();
    void set_PID_parameters(struct_PID_parameters params);
    void run();
};

extern PID_block PID_block_component;


#endif
