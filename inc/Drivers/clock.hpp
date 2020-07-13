#ifndef CLOCK_HPP
#define CLOCK_HPP

#pragma once
#include "stm32f3xx_hal.h"

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void);

/**
  * Initializes the Global MSP.
  */
void hal_clock_init(void);


void gpio_clock_init(GPIO_TypeDef *gpio_port);

void uart_clock_init(USART_TypeDef *uart_instance);

void adc_clock_init(ADC_TypeDef *adc_instance);

void dma_clock_init();

void pwm_timer_clock_init(TIM_TypeDef *timer_instance);

#endif // CLOCK_HPP
