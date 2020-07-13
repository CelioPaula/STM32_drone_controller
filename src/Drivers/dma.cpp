#include "Drivers/dma.hpp"

DMA_HandleTypeDef dma_handle = {0};

Dma::Dma(DMA_Channel_TypeDef *dma_instance, DMA_Priority dma_priority) :
    dma_instance(dma_instance),
    dma_priority(dma_priority) {
    set_dma();
}

void Dma::init() {
    dma_clock_init();
    HAL_DMA_Init(&dma_handle);
}

void Dma::set_dma() {
    dma_handle.Instance = dma_instance;
    dma_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;
    dma_handle.Init.PeriphInc = DMA_PINC_DISABLE;
    dma_handle.Init.MemInc = DMA_MINC_ENABLE;
    dma_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    dma_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    dma_handle.Init.Mode = DMA_CIRCULAR;
    dma_handle.Init.Priority = dma_priority;
}

void Dma::set_dma_interrupts(uint32_t preempt_priority, uint32_t sub_priority) {
    HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, preempt_priority, sub_priority);
    HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}

extern "C" {
    void DMA1_Channel1_IRQHandler(void)
    {
      HAL_DMA_IRQHandler(&dma_handle);
    }

    void DMA2_Channel1_IRQHandler(void)
    {
      HAL_DMA_IRQHandler(&dma_handle);
    }
}
