#pragma once
#ifndef MPU_READER_HPP
#define MPU_READER_HPP

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Arduino.h>
#include <global.hpp>

class mpu_reader
{
private:

    const char* TAG = "MPU READER";
    const float sample_time;
    const uint8_t priority;

    Adafruit_MPU6050* mpu;
    TaskHandle_t mpu_reader_task_handle;

    struct{
        QueueHandle_t queue;
        struct_mpu_reader values;
    } mpu_values;

    Adafruit_Sensor *mpu_accel;
    Adafruit_Sensor *mpu_gyro; 

    void sensors_setup();

    void mpu_reading();

    static void mpu_reading_wrapper(void* arg);
    
public:

    TaskHandle_t get_task_handle();

    QueueHandle_t get_mpu_values_queue();

    void run();
    
    mpu_reader(float sample_time = 0.01, uint8_t priority = 8);

    ~mpu_reader();
};

extern mpu_reader mpu_reader_component;

#endif
