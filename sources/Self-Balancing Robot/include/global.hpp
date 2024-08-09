#pragma once
#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <Arduino.h>

struct struct_PID_parameters
{
    float Kp;
    float Ki;
    float Kd;
    float setpoint;
};

#endif