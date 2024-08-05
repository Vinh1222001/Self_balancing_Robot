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

extern TaskHandle_t motor_controller_task_handle; 

void motor_controller_init(void);
void motor_controller_run(void);


void motor_write_state(uint8_t state);
void motor_write_ENA(int value);
void motor_write_ENB(int value);
void motor_write_both_EN(int value);

void motor_stop();
#endif
