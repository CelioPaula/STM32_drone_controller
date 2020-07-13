#include "Drivers/clock.hpp"


void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    //Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    //Error_Handler();
  }
}

void hal_clock_init(void) {
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_0);
}

void gpio_clock_init(GPIO_TypeDef *gpio_port) {

    if(gpio_port == GPIOA) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
    }
    if(gpio_port == GPIOB) {
        __HAL_RCC_GPIOB_CLK_ENABLE();
    }
    if(gpio_port == GPIOC) {
        __HAL_RCC_GPIOC_CLK_ENABLE();
    }
    if(gpio_port == GPIOD) {
        __HAL_RCC_GPIOD_CLK_ENABLE();
    }
}

void uart_clock_init(USART_TypeDef *uart_instance) {
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    if (uart_instance == USART1) {
        __HAL_RCC_USART1_CLK_ENABLE();
        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
        PeriphClkInit.Usart2ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
    }
    if (uart_instance == USART2) {
        __HAL_RCC_USART2_CLK_ENABLE();
        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
        PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
    }
    if (uart_instance == USART3) {
        __HAL_RCC_USART3_CLK_ENABLE();
        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART3;
        PeriphClkInit.Usart2ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
    }

    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);
}

void adc_clock_init(ADC_TypeDef *adc_instance) {
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
    if (adc_instance == ADC1 || adc_instance == ADC2) {
        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC12;
        PeriphClkInit.Adc12ClockSelection = RCC_ADC12PLLCLK_DIV1;
        __HAL_RCC_ADC12_CLK_ENABLE();
    }
    if (adc_instance == ADC3 || adc_instance == ADC4) {
        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC34;
        PeriphClkInit.Adc34ClockSelection = RCC_ADC34PLLCLK_DIV1;
        __HAL_RCC_ADC34_CLK_ENABLE();
    }
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);
}

void dma_clock_init() {
    /* DMA controller clock enable */
    __HAL_RCC_DMA1_CLK_ENABLE();
    __HAL_RCC_DMA2_CLK_ENABLE();
}

void pwm_timer_clock_init(TIM_TypeDef *timer_instance) {
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
    if (timer_instance == TIM2) {
        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_TIM2;
        PeriphClkInit.Tim2ClockSelection = RCC_TIM2CLK_HCLK;
    }
    if (timer_instance == TIM1) {
        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_TIM1;
        PeriphClkInit.Tim1ClockSelection = RCC_TIM1CLK_HCLK;
    }
    if (timer_instance == TIM3 || timer_instance == TIM4) {
        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_TIM34;
        PeriphClkInit.Tim34ClockSelection = RCC_TIM34CLK_HCLK;
    }
    if (timer_instance == TIM8) {
        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_TIM8;
        PeriphClkInit.Tim8ClockSelection = RCC_TIM8CLK_HCLK;
    }
    if (timer_instance == TIM15) {
        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_TIM15;
        PeriphClkInit.Tim15ClockSelection = RCC_TIM15CLK_HCLK;
    }
    if (timer_instance == TIM16) {
        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_TIM16;
        PeriphClkInit.Tim16ClockSelection = RCC_TIM16CLK_HCLK;
    }
    if (timer_instance == TIM17) {
        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_TIM17;
        PeriphClkInit.Tim17ClockSelection = RCC_TIM17CLK_HCLK;
    }
    if (timer_instance == TIM20) {
        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_TIM20;
        PeriphClkInit.Tim20ClockSelection = RCC_TIM20CLK_HCLK;
    }
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
  // Peripheral clock enable
  if(htim_base->Instance==TIM1)
  {
    __HAL_RCC_TIM1_CLK_ENABLE();
  }
  if(htim_base->Instance==TIM2)
  {
    __HAL_RCC_TIM2_CLK_ENABLE();
  }
  if(htim_base->Instance==TIM3)
  {
    __HAL_RCC_TIM3_CLK_ENABLE();
  }
  if(htim_base->Instance==TIM4)
  {
    __HAL_RCC_TIM4_CLK_ENABLE();
  }
  if(htim_base->Instance==TIM8)
  {
    __HAL_RCC_TIM8_CLK_ENABLE();
  }
  if(htim_base->Instance==TIM15)
  {
    __HAL_RCC_TIM15_CLK_ENABLE();
  }
  if(htim_base->Instance==TIM16)
  {
    __HAL_RCC_TIM16_CLK_ENABLE();
  }
  if(htim_base->Instance==TIM17)
  {
    __HAL_RCC_TIM17_CLK_ENABLE();
  }
  if(htim_base->Instance==TIM20)
  {
    __HAL_RCC_TIM20_CLK_ENABLE();
  }
}
