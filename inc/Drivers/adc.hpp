#ifndef ADC_HPP
#define ADC_HPP

#pragma once
#include "stm32f3xx_hal.h"
#include "Drivers/clock.hpp"
#include "Drivers/gpio.hpp"
#include "Drivers/dma.hpp"

typedef enum {
    ADC_Resolution_6B = ADC_RESOLUTION_6B,
    ADC_Resolution_8B = ADC_RESOLUTION_8B,
    ADC_Resolution_10B = ADC_RESOLUTION_10B,
    ADC_Resolution_12B = ADC_RESOLUTION_12B,
}ADC_Resolution;

class Adc {

    public :

        Adc(GPIO_TypeDef *gpio_port, uint16_t pin_number, ADC_Resolution adc_resolution, bool use_dma);

        void init();

        bool init_adc_dma(DMA_Priority dma_priority, uint32_t dma_preempt_priority, uint32_t dma_sub_priority);

        void start_adc();

        uint32_t get_adc_value();

    private :

        void init_pinouts();

        void set_adc_channel();

        void set_gpioa_channel();

        void set_gpiob_channel();

        void set_gpioc_channel();

        void set_multimode();

        void set_adc_config();

        DMA_Channel_TypeDef *get_adc_dma_channel();

        GPIO_TypeDef *gpio_port;

        uint16_t pin_number;

        ADC_Resolution resolution;

        bool use_dma;

        GPIO_InitTypeDef gpio_init_struct = {0};


        ADC_HandleTypeDef adc_handle = {0};

        ADC_MultiModeTypeDef multimode = {0};

        ADC_ChannelConfTypeDef s_config = {0};

        ADC_TypeDef *instance;

        uint32_t channel;

#define MAX_ADC_DEVICES 1

        uint32_t dma_buffer[MAX_ADC_DEVICES];
};


#endif // ADC_HPP
