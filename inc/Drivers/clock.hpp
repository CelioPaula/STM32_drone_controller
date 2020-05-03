#ifndef CLOCK_HPP
#define CLOCK_HPP

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

#endif // CLOCK_HPP
