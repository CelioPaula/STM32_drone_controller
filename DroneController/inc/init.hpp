#pragma once

#include "stm32f4xx_hal.h"
#include "Drivers/clock.hpp"
#include "Drivers/timer.hpp"
#include "Drivers/gpio.hpp"
#include "Drivers/uart.hpp"
#include "Drivers/adc.hpp"
#include "Drivers/dma.hpp"
#include "Drivers/i2c.hpp"
#include "Drivers/adc.hpp"
#include "actuators/brushless.hpp"
#include "sensors/imu.hpp"
#include "controllers/PID.hpp"

extern Timer IMU_scheduler;
extern PID pid_roll;
extern PID pid_pitch;

//extern Adc adc_input;

extern Brushless motor_FR;
extern Brushless motor_BR;
extern Brushless motor_FL;
extern Brushless motor_BL;


extern IMU mpu6050;

void init_all();
