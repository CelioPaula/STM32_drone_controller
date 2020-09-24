#include "Drivers/clock.hpp"


void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
    */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
    /** Initializes the CPU, AHB and APB busses clocks
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 16;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);
    /** Initializes the CPU, AHB and APB busses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);

    // RCC Crystal oscillator clock enable
    __HAL_RCC_GPIOH_CLK_ENABLE();
}

void HAL_MspInit(void) {
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
}

void uart_clock_init(USART_TypeDef *uart_instance) {
    if (uart_instance == USART1) {
        __HAL_RCC_USART1_CLK_ENABLE();
    }
    if (uart_instance == USART2) {
        __HAL_RCC_USART2_CLK_ENABLE();
    }
    if (uart_instance == USART6) {
        __HAL_RCC_USART6_CLK_ENABLE();
    }
}

void adc_clock_init(ADC_TypeDef *adc_instance) {
    if (adc_instance == ADC1) {
        __HAL_RCC_ADC1_CLK_ENABLE();
    }
}

void dma_clock_init() {
    /* DMA controller clock enable */
    __HAL_RCC_DMA1_CLK_ENABLE();
    __HAL_RCC_DMA2_CLK_ENABLE();
}

void i2c_clock_init(I2C_TypeDef *i2c_instance) {
    if (i2c_instance == (I2C_TypeDef *)I2C1) {
        __HAL_RCC_I2C1_CLK_ENABLE();
    }
    if (i2c_instance == (I2C_TypeDef *)I2C2) {
        __HAL_RCC_I2C2_CLK_ENABLE();
    }
    if (i2c_instance == (I2C_TypeDef *)I2C3) {
        __HAL_RCC_I2C3_CLK_ENABLE();
    }
}


// TODO : change into timer_clock_init()
void timer_clock_init(TIM_TypeDef *timer_instance)
{
  // Peripheral clock enable
  if(timer_instance == TIM1)
  {
    __HAL_RCC_TIM1_CLK_ENABLE();
  }
  if(timer_instance == TIM2)
  {
    __HAL_RCC_TIM2_CLK_ENABLE();
  }
  if(timer_instance == TIM3)
  {
    __HAL_RCC_TIM3_CLK_ENABLE();
  }
  if (timer_instance == TIM4)
  {
    __HAL_RCC_TIM4_CLK_ENABLE();
  }
  if (timer_instance == TIM5)
  {
    __HAL_RCC_TIM5_CLK_ENABLE();
  }
}
