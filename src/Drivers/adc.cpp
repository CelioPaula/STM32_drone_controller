#include "Drivers/adc.hpp"

#include "Drivers/uart.hpp"
#include <string.h>

Adc::Adc(GPIO_TypeDef *gpio_port, uint16_t pin_number, ADC_Resolution adc_resolution, bool use_dma):
    max_converted_value(0),
    gpio_port(gpio_port), pin_number(pin_number),
    resolution(adc_resolution),
    use_dma(use_dma) {

    adc_handle.Instance = ADC1;
    adc_handle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    adc_handle.Init.Resolution = resolution;
    adc_handle.Init.ScanConvMode = DISABLE;
    adc_handle.Init.ContinuousConvMode = ENABLE;
    adc_handle.Init.DiscontinuousConvMode = DISABLE;
    adc_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    adc_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    adc_handle.Init.NbrOfConversion = 1;
    if (use_dma) {
        adc_handle.Init.DMAContinuousRequests = ENABLE;
    } else {
        adc_handle.Init.DMAContinuousRequests = DISABLE;
    }
    adc_handle.Init.EOCSelection = EOC_SEQ_CONV;

    init_pinouts();

    //set_multimode();

    set_adc_config();

    set_adc_max_converted_value();

    memset(dma_buffer, 0, MAX_ADC_DEVICES * sizeof(uint32_t));
}

void Adc::init() {
    gpio_clock_init(gpio_port);

    adc_clock_init(ADC1);

    HAL_GPIO_Init(gpio_port, &gpio_init_struct);

    HAL_ADC_Init(&adc_handle);

    //HAL_ADCEx_MultiModeConfigChannel(&adc_handle, &multimode);

    HAL_ADC_ConfigChannel(&adc_handle, &s_config);
}

void Adc::init(DMA_Priority dma_priority, uint32_t dma_preempt_priority, uint32_t dma_sub_priority) {
    if (use_dma) {
        init();
        // DMA instance could be either DMA2_Stream0 or DMA2_Stream4
        DMA_Stream_TypeDef *dma_instance = DMA2_Stream0;
        // DMA Channel
        uint32_t dma_channel = DMA_CHANNEL_0;
        Dma dma = Dma(dma_instance, dma_channel, dma_priority);
        set_adc_dma_interrupts(dma_preempt_priority, dma_sub_priority);
        dma.init();
        __HAL_LINKDMA(&adc_handle,DMA_Handle,dma_handle);
    }
}

void Adc::init_pinouts() {
    gpio_init_struct.Pin = pin_number;
    gpio_init_struct.Mode = GPIO_MODE_ANALOG;
    gpio_init_struct.Pull = GPIO_NOPULL;
}

void Adc::set_multimode() {
    multimode.Mode = ADC_MODE_INDEPENDENT;
}

void Adc::set_adc_config() {
    s_config.Channel = set_adc_channel();
    s_config.Rank = 1;
    s_config.SamplingTime = ADC_SAMPLETIME_480CYCLES;
    s_config.Offset = 0;
}

uint32_t Adc::set_adc_channel() {
    if (pin_number == GPIO_PIN_0) {
        return ADC_CHANNEL_0;
    }
    if (pin_number == GPIO_PIN_1) {
        return ADC_CHANNEL_1;
    }
    if (pin_number == GPIO_PIN_2) {
        return ADC_CHANNEL_2;
    }
    if (pin_number == GPIO_PIN_3) {
        return ADC_CHANNEL_3;
    }
    if (pin_number == GPIO_PIN_4) {
        return ADC_CHANNEL_4;
    }
    if (pin_number == GPIO_PIN_5) {
        return ADC_CHANNEL_5;
    }
    if (pin_number == GPIO_PIN_6) {
        return ADC_CHANNEL_6;
    }
    if (pin_number == GPIO_PIN_7) {
        return ADC_CHANNEL_7;
    }
    if (pin_number == GPIO_PIN_8) {
        return ADC_CHANNEL_8;
    }
    if (pin_number == GPIO_PIN_9) {
        return ADC_CHANNEL_9;
    }
    if (pin_number == GPIO_PIN_10) {
        return ADC_CHANNEL_10;
    }
    if (pin_number == GPIO_PIN_11) {
        return ADC_CHANNEL_11;
    }
    if (pin_number == GPIO_PIN_12) {
        return ADC_CHANNEL_12;
    }
    if (pin_number == GPIO_PIN_13) {
        return ADC_CHANNEL_13;
    }
    if (pin_number == GPIO_PIN_14) {
        return ADC_CHANNEL_14;
    }
    if (pin_number == GPIO_PIN_15) {
        return ADC_CHANNEL_15;
    }
    return 0;
}

void Adc::set_adc_dma_interrupts(uint32_t preempt_priority, uint32_t sub_priority) {
    // For ADC1 DMA interrupt : DMA2_Stream0_IRQn or DMA2_Stream4_IRQn
    HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, preempt_priority, sub_priority);
    HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
}

void Adc::set_adc_max_converted_value() {
    if (resolution == ADC_Resolution_6B) {
        max_converted_value = 63;
    }
    if (resolution == ADC_Resolution_8B) {
        max_converted_value = 255;
    }
    if (resolution == ADC_Resolution_10B) {
        max_converted_value = 1023;
    }
    if (resolution == ADC_Resolution_12B) {
        max_converted_value = 4095;
    }
}

void Adc::start_adc() {
    if (use_dma) {
        HAL_ADC_Start_DMA(&adc_handle, dma_buffer, MAX_ADC_DEVICES);
    } else {
        HAL_ADC_Start(&adc_handle);
    }
}

uint32_t Adc::get_adc_value() {
    if (HAL_ADC_PollForConversion(&adc_handle, 1000) == HAL_OK) {
        return HAL_ADC_GetValue(&adc_handle);
    } else {
        return 0;
    }
}

