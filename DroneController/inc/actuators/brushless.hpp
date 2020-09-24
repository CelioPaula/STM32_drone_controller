#pragma once
#include "stm32f4xx_hal.h"
#include "Drivers/gpio.hpp"

class Brushless {

    public :
        Brushless(GPIO_TypeDef *gpio_port, uint16_t pin_number, uint32_t max_speed_value, uint32_t speed_offset_value);

        void set_speed(uint32_t desired_speed);

        void set_speed(uint32_t desired_speed, uint32_t roll_correction, uint32_t pitch_correction);

        void init();

    private :
        // SERVO PWM 50Hz (5% to 10% duty cycle)
        // SERVO HS 500Hz (TO CHECK)
        uint32_t pwm_frequency = 50;
        const uint32_t timer_period = 1000;
        const uint32_t timer_prescaler = 1680;
        const uint32_t pwm_period = 2000;
        const uint32_t min_pwm_time_high = 1000;

        uint32_t speed_value;
        Pwm_output pwm_output;
        uint32_t max_speed_value;
        uint32_t speed_offset_value;

};
