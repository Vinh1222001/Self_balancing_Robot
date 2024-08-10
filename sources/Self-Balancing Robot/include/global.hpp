#pragma once
#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <Arduino.h>
#include <Adafruit_Sensor.h>

struct struct_PID_parameters
{
    float Kp;
    float Ki;
    float Kd;
    float setpoint;
};

struct struct_PID_block
{
    float filted_pitch;
    float gyro_angle_Y;
    float output;
    float error;
    float integral;
    float derivative;
};

struct struct_mpu_reader{
    // float roll;
    sensors_vec_t accel;
    sensors_vec_t gyro;
};

struct struct_eeprom_address
{
    int eeprom_Kp_address;
    int eeprom_Ki_address;
    int eeprom_Kd_address;
    int eeprom_setpoint_address;

};

struct struct_angle_values{
    float pitch;
    float gyroY;
};



#endif