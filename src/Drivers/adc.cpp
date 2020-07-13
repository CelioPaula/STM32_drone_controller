#include "Drivers/adc.hpp"

#include <string.h>

Adc::Adc(GPIO_TypeDef *gpio_port, uint16_t pin_number, ADC_Resolution adc_resolution, bool use_dma):
    gpio_port(gpio_port), pin_number(pin_number),
    resolution(adc_resolution),
    use_dma(use_dma) {

    set_adc_channel();

    adc_handle.Instance = instance;
    adc_handle.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
    adc_handle.Init.Resolution = resolution;
    adc_handle.Init.ScanConvMode = ADC_SCAN_DISABLE;
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
    adc_handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    adc_handle.Init.LowPowerAutoWait = ENABLE;
    adc_handle.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;

    set_multimode();

    set_adc_config();

    memset(dma_buffer, 0, MAX_ADC_DEVICES * sizeof(uint32_t));
}

void Adc::init() {

    gpio_clock_init(gpio_port);

    adc_clock_init(instance);

    HAL_GPIO_Init(gpio_port, &gpio_init_struct);

    HAL_ADC_Init(&adc_handle);

    HAL_ADCEx_MultiModeConfigChannel(&adc_handle, &multimode);

    HAL_ADC_ConfigChannel(&adc_handle, &s_config);
}

bool Adc::init_adc_dma(DMA_Priority dma_priority, uint32_t dma_preempt_priority, uint32_t dma_sub_priority) {
    if (use_dma) {
        Dma dma = Dma(get_adc_dma_channel(), dma_priority);
        dma.init();
        __HAL_LINKDMA(&adc_handle,DMA_Handle,dma_handle);
        dma.set_dma_interrupts(dma_preempt_priority, dma_sub_priority);
        return true;
    } else {
        return false;
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
    s_config.Channel = channel;
    s_config.Rank = ADC_REGULAR_RANK_1;
    s_config.SingleDiff = ADC_SINGLE_ENDED;
    s_config.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
    s_config.OffsetNumber = ADC_OFFSET_NONE;
    s_config.Offset = 0;
}

void Adc::set_adc_channel() {
    if (gpio_port == GPIOA) {
        set_gpioa_channel();
    }
    if (gpio_port == GPIOB) {
        set_gpiob_channel();
    }
    if (gpio_port == GPIOC) {
        set_gpioc_channel();
    }
}

void Adc::set_gpioa_channel() {
    if (pin_number == GPIO_PIN_0) {
        channel = ADC_CHANNEL_1;
        instance = ADC1;
    }
    if (pin_number == GPIO_PIN_1) {
        channel = ADC_CHANNEL_2;
        instance = ADC1;
    }
    if (pin_number == GPIO_PIN_2) {
        channel = ADC_CHANNEL_3;
        instance = ADC1;
    }
    if (pin_number == GPIO_PIN_3) {
        channel = ADC_CHANNEL_4;
        instance = ADC1;
    }
    if (pin_number == GPIO_PIN_4) {
        channel = ADC_CHANNEL_1;
        instance = ADC2;
    }
    if (pin_number == GPIO_PIN_5) {
        channel = ADC_CHANNEL_2;
        instance = ADC2;
    }
    if (pin_number == GPIO_PIN_6) {
        channel = ADC_CHANNEL_3;
        instance = ADC2;
    }
    if (pin_number == GPIO_PIN_7) {
        channel = ADC_CHANNEL_4;
        instance = ADC2;
    }
}

void Adc::set_gpiob_channel() {
    if (pin_number == GPIO_PIN_0) {
        channel = ADC_CHANNEL_12;
        instance = ADC3;
    }
    if (pin_number == GPIO_PIN_1) {
        channel = ADC_CHANNEL_1;
        instance = ADC3;
    }
    if (pin_number == GPIO_PIN_2) {
        channel = ADC_CHANNEL_12;
        instance = ADC2;
    }
    if (pin_number == GPIO_PIN_11) {
        channel = ADC_CHANNEL_14;
        instance = ADC1;
    }
    if (pin_number == GPIO_PIN_12) {
        channel = ADC_CHANNEL_3;
        instance = ADC4;
    }
    if (pin_number == GPIO_PIN_13) {
        channel = ADC_CHANNEL_5;
        instance = ADC3;
    }
    if (pin_number == GPIO_PIN_14) {
        channel = ADC_CHANNEL_4;
        instance = ADC4;
    }
    if (pin_number == GPIO_PIN_15) {
        channel = ADC_CHANNEL_5;
        instance = ADC4;
    }
}

void Adc::set_gpioc_channel() {
    if (pin_number == GPIO_PIN_0) {
        channel = ADC_CHANNEL_6;
        instance = ADC1;
    }
    if (pin_number == GPIO_PIN_1) {
        channel = ADC_CHANNEL_7;
        instance = ADC1;
    }
    if (pin_number == GPIO_PIN_2) {
        channel = ADC_CHANNEL_8;
        instance = ADC1;
    }
    if (pin_number == GPIO_PIN_3) {
        channel = ADC_CHANNEL_9;
        instance = ADC1;
    }
    if (pin_number == GPIO_PIN_4) {
        channel = ADC_CHANNEL_5;
        instance = ADC2;
    }
    if (pin_number == GPIO_PIN_5) {
        channel = ADC_CHANNEL_11;
        instance = ADC2;
    }
}

DMA_Channel_TypeDef *Adc::get_adc_dma_channel() {
    DMA_Channel_TypeDef *dma_instance = {0};
    if (instance == ADC1) {
        dma_instance = DMA1_Channel1;
    }
    if (instance == ADC2) {
        // Could be use either DMA2_Channel1 or DMA2_Channel3
        dma_instance = DMA2_Channel1;
    }
    if (instance == ADC3) {
        dma_instance = DMA2_Channel5;
    }
    if (instance == ADC4) {
        // Could be use either DMA2_Channel2 or DMA2_Channel4
        dma_instance = DMA2_Channel2;
    }
    return dma_instance;
}

void Adc::start_adc() {
    if (use_dma) {
        HAL_ADC_Start_DMA(&adc_handle, dma_buffer, MAX_ADC_DEVICES);
    } else {
        HAL_ADC_Start(&adc_handle);
    }
}

uint32_t Adc::get_adc_value() {
    HAL_ADC_PollForConversion(&adc_handle, 1000);
    return HAL_ADC_GetValue(&adc_handle);
}


