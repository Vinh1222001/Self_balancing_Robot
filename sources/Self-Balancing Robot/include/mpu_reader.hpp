#pragma once
#ifndef MPU_READER_HPP
#define MPU_READER_HPP

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Arduino.h>

extern Adafruit_MPU6050* mpu_reader;

struct struct_mpu_reader{
    // float roll;
    sensors_vec_t accel;
    sensors_vec_t gyro;
};

extern QueueHandle_t q_mpu_values;

void mpu_reader_init(void);

void mpu_reader_run();

#endif
