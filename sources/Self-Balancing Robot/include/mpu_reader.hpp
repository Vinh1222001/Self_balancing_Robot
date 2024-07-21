#pragma once
#ifndef MPU_READER_HPP
#define MPU_READER_HPP

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Arduino.h>

extern Adafruit_MPU6050* mpu_reader;

void init_mpu_reader(void);

void mpu_reading(void* arg);

#endif
