#pragma once
#ifndef CENTER_CONTROLLER_HPP
#define CENTER_CONTROLLER_HPP

#include <Arduino.h>
#include "EEPROM.h"
#include "mpu_reader.hpp"
#include "motor_controller.hpp"
#include "PID_block.hpp"
#include "display_controller.hpp"
#include "global.hpp"

class center_controller
{
private:

    const char* TAG = "CENTER CONTROLLER";
    const int delay_time;

    struct{

        bool flag;
        SemaphoreHandle_t xMutex_flag;

    } start_robot_flag;

    struct{

        struct_PID_parameters params;
        SemaphoreHandle_t xMutex_PID_parameters;

    } PID_params;
    

    struct_eeprom_address eeprom_adresses;

    // QueueHandle_t q_angle_values;

    // struct_angle_values angle_values;

    void system_controller();

    static void system_controller_wrapper(void* arg);


public:
    center_controller(int delay_time = 200);
    ~center_controller();

    void set_PID_parameters(struct_PID_parameters params);

    struct_PID_parameters get_PID_parameters();

    SemaphoreHandle_t get_mutex_PID_params_handle();

    struct_eeprom_address get_eeprom_adresses();

    void set_start_robot_flag(bool value);

    void run();
};

extern center_controller* center_controller_component;

#endif
