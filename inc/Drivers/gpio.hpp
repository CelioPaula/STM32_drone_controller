#ifndef GPIO_HPP
#define GPIO_HPP

#pragma once
#include "stm32f4xx_hal.h"
#include "Drivers/clock.hpp"


typedef enum {
    No_Pull = GPIO_NOPULL,
    Pull_Up = GPIO_PULLUP,
    Pull_Down = GPIO_PULLDOWN,
}Pull_Mode;

typedef enum {
    FloatingInput = GPIO_MODE_INPUT,
    AnalogInput = GPIO_MODE_ANALOG,
    EXTIT_RisingEdge = GPIO_MODE_IT_RISING,
    EXTIT_FallingEdge = GPIO_MODE_IT_FALLING,
}Input_Mode;


typedef enum {
    GPIO_State_LOW = GPIO_PIN_SET,
    GPIO_State_HIGH = GPIO_PIN_RESET,
}GPIO_State;

typedef enum {
    SpeedUpTo_2MHz = GPIO_SPEED_FREQ_LOW,
    SpeedUpTo_10MHz = GPIO_SPEED_FREQ_MEDIUM,
    SpeedUpTo_50MHz = GPIO_SPEED_FREQ_HIGH,
}Output_Speed;

typedef enum {
     PushPull = GPIO_MODE_OUTPUT_PP,
     OpenDrain = GPIO_MODE_OUTPUT_OD,
     AlternatePushPull = GPIO_MODE_AF_PP
}Output_Mode;

class Gpio
{
    public:

        GPIO_TypeDef* gpio_port;

        uint16_t pin_number;

    protected:

        Gpio(GPIO_TypeDef *gpio_port, uint16_t gpio_pin);

        GPIO_InitTypeDef gpio_init_struct;

        GPIO_State gpio_state;
};

class Gpio_input : public Gpio
{
    public:

       Gpio_input(GPIO_TypeDef *gpio_port, uint16_t gpio_pin, Input_Mode input_mode, Pull_Mode pull_mode);

       void init();

       GPIO_State get_state();

       bool set_interrupt_priority(uint32_t preempt_priority, uint32_t sub_priority);

    private:

       Input_Mode input_mode;

       Pull_Mode pull_mode;
};

class Gpio_output : public Gpio
{
    public:

        Gpio_output(GPIO_TypeDef *gpio_port, uint16_t gpio_pin, Output_Mode output_mode, Output_Speed output_speed);

        void init();

        void set_state(GPIO_State pin_state);

        void toggle_state();

    private:

        Output_Mode output_mode;

        Output_Speed output_speed;
};

class Pwm_output : public Gpio_output {
    public:

        Pwm_output(GPIO_TypeDef *gpio_port, uint16_t pin_number, uint16_t timer_period, uint16_t timer_prescaler);

        void init();

        void start();

        void set_duty_cycle(float duty_cyle);

        uint32_t pwm_frequency;
        uint32_t duty_cycle;

        static TIM_HandleTypeDef timer_handle;

        TIM_TypeDef *timer_instance;

        uint32_t timer_channel;

    private:

        void set_pwm_timer_instance();

        void set_alternate_function();

        void init_pwm_timer_struct();

        void init_pwm_timer_clock_source_config();

        void init_pwm_timer_output_compare();

        void init_pwm_timer_master_config();



        uint16_t period;

        uint16_t prescaler;
};

#endif // GPIO_HPP
