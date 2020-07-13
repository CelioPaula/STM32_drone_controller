#ifndef DMA_HPP
#define DMA_HPP

#pragma once
#include "stm32f3xx_hal.h"
#include "Drivers/clock.hpp"

typedef enum {
    DMA_Priority_Low = DMA_PRIORITY_LOW,
    DMA_Priority_Med = DMA_PRIORITY_MEDIUM,
    DMA_Priority_High = DMA_PRIORITY_HIGH,
    DMA_Priority_VHigh = DMA_PRIORITY_VERY_HIGH,
}DMA_Priority;

extern DMA_HandleTypeDef dma_handle;

class Dma
{
    public:

        Dma(DMA_Channel_TypeDef *dma_instance, DMA_Priority dma_priority);

        void set_dma();

        void init();

        void set_dma_interrupts(uint32_t preempt_priority, uint32_t sub_priority);

    private :

        DMA_Channel_TypeDef *dma_instance;

        DMA_Priority dma_priority;

};

#endif // DMA_HPP
