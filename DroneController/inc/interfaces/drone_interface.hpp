#pragma once
#include "stm32f4xx_hal.h"

typedef struct {
    float pitch;
    float roll;
    float yaw;
    float throttle;
    float battery_avoltage;
}Drone_State;

typedef struct {
    float pitch;
    float roll;
    float yaw;
    float throttle;
}Commands;

typedef struct {
    float kp;
    float ki;
    float kd;
}PID_Configuration;

extern Drone_State drone_state;
extern Commands commands;
extern PID_Configuration pid_pitch_configuration;
extern PID_Configuration pid_roll_configuration;
extern PID_Configuration pid_yaw_configuration;

void print_commands();
