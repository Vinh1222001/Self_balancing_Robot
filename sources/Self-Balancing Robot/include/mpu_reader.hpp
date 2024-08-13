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
    const int8_t sample_time;
    const uint8_t priority;

    Adafruit_MPU6050* mpu;
    TaskHandle_t mpu_reader_task_handle;

    struct{
        // QueueHandle_t queue;
        SemaphoreHandle_t bi_semaphore;
        struct_mpu_reader values;
    } mpu_values;

    Adafruit_Sensor *mpu_accel;
    Adafruit_Sensor *mpu_gyro; 

    void sensors_setup();

    void mpu_reading();

    static void mpu_reading_wrapper(void* arg);
    
public:

    TaskHandle_t get_task_handle();

    // QueueHandle_t get_mpu_values_queue();

    void run();
    
    SemaphoreHandle_t get_semaphore_mpu();

    struct_mpu_reader get_mpu_values();

    mpu_reader(int8_t sample_time = 20, uint8_t priority = 6);

    ~mpu_reader();
};

extern mpu_reader* mpu_reader_component;

#endif
