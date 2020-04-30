#ifndef GPIO_HPP
#define GPIO_HPP

#pragma once
#include "stm32f3xx_hal.h"

class gpio
{
    public:
        gpio();

        void init();

    private:
        uint32_t gpio_mode;
        GPIO_InitTypeDef gpio_init_struct;

    protected:

        uint8_t pin_number;
        GPIO_TypeDef* gpio_port;

        GPIO_PinState pin_state;
};

class gpio_input : public gpio
{
    public:
       void get_state();

    private:
       uint32_t pull_mode;
       uint32_t input_data;


};

class gpio_output : public gpio
{
    public:

        void set_state(GPIO_PinState pin_state);

        void toogle_state();

    private:
        uint32_t output_type;
        uint32_t output_speed;
        uint32_t output_data;
};

#endif // GPIO_HPP
