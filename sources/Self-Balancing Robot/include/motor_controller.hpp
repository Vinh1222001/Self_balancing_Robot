#pragma once
#ifndef MOTOR_CONTROLLER_HPP
#define MOTOR_CONTROLLER_HPP

#include <Arduino.h>

#define MOTOR_IN1_PIN 19
#define MOTOR_IN2_PIN 18
#define MOTOR_IN3_PIN 17
#define MOTOR_IN4_PIN 16

#define MOTOR_ENA_PIN 25
#define MOTOR_ENB_PIN 26

#define MOVE_FORWARD    0
#define MOVE_BACKWOARD  1
#define MOVE_LEFT       2
#define MOVE_RIGHT      3
#define MOVE_STOP       4

// extern TaskHandle_t motor_controller_task_handle; 

// void motor_controller_init(void);
// void motor_controller_run(void);


// void motor_write_state(uint8_t state);
// void motor_write_ENA(int value);
// void motor_write_ENB(int value);
// void motor_write_both_EN(int value);

// void motor_stop();

class motor_controller
{
private:

    const char* TAG = "MOTOR CONTROLLER";
    const uint8_t motor_in_state[5][4];
    const int delay_time;
    const uint8_t priority;

    TaskHandle_t motor_controller_task_handle;
    
    struct{

        uint8_t state;
        SemaphoreHandle_t xMutex_state;

    }current_movement_state;

    struct{

        int value;
        SemaphoreHandle_t xMutex_value;

    }ENA_value;
    
    struct{

        int value;
        SemaphoreHandle_t xMutex_value;

    }ENB_value;

    void motor_work();

    static void motor_work_wrapper(void* arg);

public:
    motor_controller(int delay_time = 10, uint8_t priority = 5);
    ~motor_controller();

    void run(void);

    TaskHandle_t get_task_hanlde();

    void write_state(uint8_t state);
    void write_ENA(int value);
    void write_ENB(int value);
    void write_both_EN(int value);

    void stop();
};

extern motor_controller motor_controller_component;

#endif
