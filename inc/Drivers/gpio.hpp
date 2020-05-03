#ifndef GPIO_HPP
#define GPIO_HPP

#pragma once
#include "stm32f3xx_hal.h"
#include "Drivers/clock.hpp"


typedef enum {
    No_Pull,
    Pull_Up,
    Pull_Down
}Pull_Mode;

typedef enum {
    FloatingInput,
    AnalogInput,
    EXTIT_RisingEdge,
    EXTIT_FallingEdge,
}Input_Mode;


typedef enum {
    GPIO_State_LOW,
    GPIO_State_HIGH,
}GPIO_State;

typedef enum {
    SpeedUpTo_2MHz,
    SpeedUpTo_10MHz,
    SpeedUpTo_50MHz,
}Output_Speed;

typedef enum {
     Push_Pull,
     Open_Drain,
}Output_Mode;

class Gpio
{
    public:
        uint16_t pin_number;

    protected:

        Gpio(GPIO_TypeDef *gpio_port, uint16_t gpio_pin);

        void init_struct();

        GPIO_TypeDef* gpio_port;

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

#endif // GPIO_HPP
