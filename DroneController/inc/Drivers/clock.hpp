#pragma once
#include "stm32f4xx_hal.h"

#define H_CLOCK_FREQ 84000000

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void);

/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void);


void gpio_clock_init(GPIO_TypeDef *gpio_port);

void uart_clock_init(USART_TypeDef *uart_instance);

void adc_clock_init(ADC_TypeDef *adc_instance);

void dma_clock_init();

void i2c_clock_init(I2C_TypeDef *i2c_instance);

void timer_clock_init(TIM_TypeDef *timer_instance);
