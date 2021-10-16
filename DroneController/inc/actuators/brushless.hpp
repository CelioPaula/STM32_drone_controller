#pragma once
#include "stm32f4xx_hal.h"
#include "Drivers/gpio.hpp"

class Brushless {

    public :
        Brushless(GPIO_TypeDef *gpio_port, uint16_t pin_number, uint32_t speed_offset_value);

        Brushless(GPIO_TypeDef *gpio_port, uint16_t pin_number, uint32_t speed_offset_value, float speed_divider);

        void set_speed(uint32_t desired_speed);

        void set_speed(uint32_t desired_speed, float roll_correction, float pitch_correction);

        void init();

        static const uint32_t MAX_SPEED_VALUE = 2000;


    private :
        // SERVO PWM 50Hz (5% to 10% duty cycle)
        // SERVO HS 500Hz (TO CHECK)
        // PWM_FREQUENCY = 50Hz;
        const uint32_t TIMER_PERIOD = 1000;
        const uint32_t TIMER_PRESCALER = 1680;
        const uint32_t PWM_PERIOD = 20000;
        const uint32_t MIN_PWM_TIME_HIGH = 1000;
        const uint32_t MAX_PWM_TIME_HIGH = 2000;


        uint32_t speed_value;
        Pwm_output pwm_output;
        uint32_t speed_offset_value;

        float speed_divider;

};
