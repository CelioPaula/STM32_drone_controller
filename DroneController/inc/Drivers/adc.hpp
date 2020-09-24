#pragma once
#include "stm32f4xx_hal.h"
#include "Drivers/clock.hpp"
#include "Drivers/gpio.hpp"
#include "Drivers/dma.hpp"

typedef enum {
    ADC_Resolution_6B = ADC_RESOLUTION6b,
    ADC_Resolution_8B = ADC_RESOLUTION8b,
    ADC_Resolution_10B = ADC_RESOLUTION10b,
    ADC_Resolution_12B = ADC_RESOLUTION12b,
}ADC_Resolution;

class Adc {

    public :

        Adc(GPIO_TypeDef *gpio_port, uint16_t pin_number, ADC_Resolution adc_resolution, bool use_dma);

        void init();

        void init(DMA_Priority dma_priority, uint32_t dma_preempt_priority, uint32_t dma_sub_priority);

        void start_adc();

        uint32_t get_adc_value();

        void set_adc_dma_interrupts(uint32_t preempt_priority, uint32_t sub_priority);

        uint32_t max_converted_value;

#define MAX_ADC_DEVICES 1

        uint32_t dma_buffer[MAX_ADC_DEVICES];

    private :

        void init_pinouts();

        uint32_t set_adc_channel();

        void set_multimode();

        void set_adc_config();

        void set_adc_max_converted_value();

        GPIO_TypeDef *gpio_port;

        uint16_t pin_number;

        ADC_Resolution resolution;

        bool use_dma;

        GPIO_InitTypeDef gpio_init_struct = {0};

        ADC_HandleTypeDef adc_handle = {0};

        ADC_MultiModeTypeDef multimode = {0};

        ADC_ChannelConfTypeDef s_config = {0};
};
