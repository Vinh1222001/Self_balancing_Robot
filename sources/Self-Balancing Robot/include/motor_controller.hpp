#pragma once
#ifndef MOTOR_CONTROLLER_HPP
#define MOTOR_CONTROLLER_HPP

#include <Arduino.h>

#define MOTOR_IN1_PIN   19
#define MOTOR_IN2_PIN   18
#define MOTOR_IN3_PIN   17
#define MOTOR_IN4_PIN   16
    
#define MOTOR_ENA_PIN   25
#define MOTOR_ENB_PIN   26

#define MOVE_FORWARD    0
#define MOVE_BACKWARD   1
#define MOVE_LEFT       2
#define MOVE_RIGHT      3
#define MOVE_STOP       4

#define MIN_MOVE_STATE_VALUE  0
#define MAX_MOVE_STATE_VALUE  4

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
    motor_controller(int delay_time = 10, uint8_t priority = 7);
    ~motor_controller();

    void run(void);

    TaskHandle_t get_task_hanlde();

    void write_state(uint8_t state);
    void write_ENA(int value);
    void write_ENB(int value);
    void write_both_EN(int value);

    void control_state(uint8_t state);
    void control_both_EN(int value);
    void control_stop();

    void stop();
};

extern motor_controller* motor_controller_component;

#endif
