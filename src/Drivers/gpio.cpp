#include "gpio.hpp"

uint16_t gpio_interrupt_pin[7] = {0};

Gpio::Gpio(GPIO_TypeDef *gpio_port, uint16_t pin_number):
    gpio_port(gpio_port),
    pin_number(pin_number),
    gpio_state(GPIO_State_LOW){
    gpio_clock_init(gpio_port);
    gpio_init_struct = {0};
    gpio_init_struct.Pin = pin_number;
}

Gpio_input::Gpio_input(GPIO_TypeDef *gpio_port, uint16_t pin_number, Input_Mode input_mode, Pull_Mode pull_mode):
    Gpio(gpio_port, pin_number),
    input_mode(input_mode),
    pull_mode(pull_mode) {
    gpio_init_struct.Mode = input_mode;
    gpio_init_struct.Pull = pull_mode;
}

void Gpio_input::init() {
    HAL_GPIO_Init(gpio_port, &gpio_init_struct);
}

GPIO_State Gpio_input::get_state() {
    if (HAL_GPIO_ReadPin(gpio_port, pin_number) == GPIO_PIN_SET) {
        gpio_state = GPIO_State_HIGH;
    } else if (HAL_GPIO_ReadPin(gpio_port, pin_number) == GPIO_PIN_RESET) {
        gpio_state = GPIO_State_LOW;
    }
    return gpio_state;
}

bool Gpio_input::set_interrupt_priority(uint32_t preempt_priority, uint32_t sub_priority) {
    if (input_mode == EXTIT_FallingEdge || input_mode == EXTIT_RisingEdge) {
        if (pin_number == GPIO_PIN_0) {
            HAL_NVIC_SetPriority(EXTI0_IRQn, preempt_priority, sub_priority);
            HAL_NVIC_EnableIRQ(EXTI0_IRQn);
            gpio_interrupt_pin[0] = pin_number;
        }
        if (pin_number == GPIO_PIN_1) {
            HAL_NVIC_SetPriority(EXTI1_IRQn, preempt_priority, sub_priority);
            HAL_NVIC_EnableIRQ(EXTI1_IRQn);
            gpio_interrupt_pin[1] = pin_number;
        }
        if (pin_number == GPIO_PIN_2) {
            HAL_NVIC_SetPriority(EXTI2_TSC_IRQn , preempt_priority, sub_priority);
            HAL_NVIC_EnableIRQ(EXTI2_TSC_IRQn);
            gpio_interrupt_pin[2] = pin_number;
        }
        if (pin_number == GPIO_PIN_3) {
            HAL_NVIC_SetPriority(EXTI3_IRQn, preempt_priority, sub_priority);
            HAL_NVIC_EnableIRQ(EXTI3_IRQn);
            gpio_interrupt_pin[3] = pin_number;
        }
        if (pin_number == GPIO_PIN_4) {
            HAL_NVIC_SetPriority(EXTI4_IRQn, preempt_priority, sub_priority);
            HAL_NVIC_EnableIRQ(EXTI4_IRQn);
            gpio_interrupt_pin[4] = pin_number;
        }
        if (pin_number >= GPIO_PIN_5 && pin_number <= GPIO_PIN_9) {
            HAL_NVIC_SetPriority(EXTI9_5_IRQn, preempt_priority, sub_priority);
            HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
            gpio_interrupt_pin[5] = pin_number;
        }
        if (pin_number >= GPIO_PIN_10 && pin_number <= GPIO_PIN_15) {
            HAL_NVIC_SetPriority(EXTI15_10_IRQn, preempt_priority, sub_priority);
            HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
            gpio_interrupt_pin[6] = pin_number;
        }   
        return true;
    } else {
        return false;
    }
}

Gpio_output::Gpio_output(GPIO_TypeDef *gpio_port, uint16_t pin_number, Output_Mode output_mode, Output_Speed output_speed):
    Gpio(gpio_port, pin_number),
    output_mode(output_mode),
    output_speed(output_speed){
    gpio_init_struct.Mode = output_mode;
    gpio_init_struct.Speed = output_speed;
}

void Gpio_output::init() {
    HAL_GPIO_Init(gpio_port, &gpio_init_struct);
}

void Gpio_output::set_state(GPIO_State state) {
    gpio_state = state;
    if (gpio_state == GPIO_State_HIGH) {
        HAL_GPIO_WritePin(gpio_port, pin_number, GPIO_PIN_SET);
    } else if (gpio_state == GPIO_State_LOW) {
        HAL_GPIO_WritePin(gpio_port, pin_number, GPIO_PIN_RESET);
    }
}

void Gpio_output::toggle_state() {
    switch (gpio_state) {
        case GPIO_State_HIGH :
           set_state(GPIO_State_LOW);
           break;
        case GPIO_State_LOW :
           set_state(GPIO_State_HIGH);
           break;
    }
}

TIM_HandleTypeDef Pwm_output::timer_handle;

Pwm_output::Pwm_output(GPIO_TypeDef *gpio_port, uint16_t pin_number, uint16_t timer_period, uint16_t timer_prescaler):
    Gpio_output(gpio_port, pin_number, AlternatePushPull, SpeedUpTo_2MHz) {
    period = timer_period;
    prescaler = timer_prescaler;

}

void Pwm_output::set_pwm_timer_instance() {
    if (gpio_port == GPIOC) {
        if (pin_number == GPIO_PIN_3) {
            timer_instance = TIM1;
            timer_channel = TIM_CHANNEL_4;
        }
        if (pin_number == GPIO_PIN_9) {
            timer_instance = TIM8;
            timer_channel = TIM_CHANNEL_4;
        }
    }
    if (gpio_port == GPIOA) {
        if (pin_number == GPIO_PIN_0) {
            timer_instance = TIM2;
            timer_channel = TIM_CHANNEL_1;
        }
        if (pin_number == GPIO_PIN_1) {
            timer_instance = TIM2;
            timer_channel = TIM_CHANNEL_2;
        }
        if (pin_number == GPIO_PIN_4) {
            timer_instance = TIM3;
            timer_channel = TIM_CHANNEL_2;
        }
        if (pin_number == GPIO_PIN_6) {
            timer_instance = TIM3;
            timer_channel = TIM_CHANNEL_1;
        }
        if (pin_number == GPIO_PIN_11) {
            timer_instance = TIM4;
            timer_channel = TIM_CHANNEL_1;
        }
        if (pin_number == GPIO_PIN_12) {
            timer_instance = TIM4;
            timer_channel = TIM_CHANNEL_2;
        }
    }
    if (gpio_port == GPIOB) {
        if (pin_number == GPIO_PIN_0) {
            timer_instance = TIM3;
            timer_channel = TIM_CHANNEL_3;
        }
        if (pin_number == GPIO_PIN_1) {
            timer_instance = TIM3;
            timer_channel = TIM_CHANNEL_4;
        }
        if (pin_number == GPIO_PIN_10) {
            timer_instance = TIM2;
            timer_channel = TIM_CHANNEL_3;
        }
        if (pin_number == GPIO_PIN_11) {
            timer_instance = TIM2;
            timer_channel = TIM_CHANNEL_4;
        }
        if (pin_number == GPIO_PIN_15) {
            timer_instance = TIM15;
            timer_channel = TIM_CHANNEL_2;
        }
        if (pin_number == GPIO_PIN_8) {
            timer_instance = TIM4;
            timer_channel = TIM_CHANNEL_3;
        }
        if (pin_number == GPIO_PIN_9) {
            timer_instance = TIM4;
            timer_channel = TIM_CHANNEL_4;
        }
    }
}

void Pwm_output::set_alternate_function() {
    if (timer_instance == TIM1) {
        gpio_init_struct.Alternate = GPIO_AF2_TIM1;
    }
    if (timer_instance == TIM2) {
        gpio_init_struct.Alternate = GPIO_AF2_TIM2;
    }
    if (timer_instance == TIM3) {
        gpio_init_struct.Alternate = GPIO_AF2_TIM3;
    }
    if (timer_instance == TIM4) {
        gpio_init_struct.Alternate = GPIO_AF2_TIM4;
    }
    if (timer_instance == TIM8) {
        gpio_init_struct.Alternate = GPIO_AF2_TIM8;
    }
    if (timer_instance == TIM15) {
        gpio_init_struct.Alternate = GPIO_AF2_TIM15;
    }
}

void Pwm_output::init() {
    set_pwm_timer_instance();
    //pwm_timer_clock_init(timer_instance);
    init_pwm_timer_struct();
    init_pwm_timer_clock_source_config();
    HAL_TIM_PWM_Init(&timer_handle);
    init_pwm_timer_master_config();
    init_pwm_timer_output_compare();

    set_alternate_function();
    Gpio_output::init();
}

void Pwm_output::init_pwm_timer_struct() {
    /** TIMER FREQUENCY = (HCLOCK FREQ)/[(PERIOD + 1)x(PRESCALER + 1)] **/
    /** Default HCLOCK FREQ : 72 MHz **/
    timer_handle.Instance = timer_instance;
    // TIMER PRESCALER : PR
    timer_handle.Init.Prescaler = prescaler;
    timer_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    // TIMER AUTORELOAD : ARR
    timer_handle.Init.Period = period;
    timer_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    timer_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_Base_Init(&timer_handle);
}

void Pwm_output::init_pwm_timer_clock_source_config() {
    TIM_ClockConfigTypeDef clock_source_config = {0};
    clock_source_config.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    HAL_TIM_ConfigClockSource(&timer_handle, &clock_source_config);
}

void Pwm_output::init_pwm_timer_output_compare() {
    TIM_OC_InitTypeDef output_compare_config = {0};
    output_compare_config.OCMode = TIM_OCMODE_PWM1;
    output_compare_config.Pulse = 0;
    output_compare_config.OCPolarity = TIM_OCPOLARITY_HIGH;
    output_compare_config.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&timer_handle, &output_compare_config, timer_channel);
}

void Pwm_output::init_pwm_timer_master_config() {
    TIM_MasterConfigTypeDef master_config = {0};
    master_config.MasterOutputTrigger = TIM_TRGO_RESET;
    master_config.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&timer_handle, &master_config);
}

void Pwm_output::set_duty_cycle(float duty_cyle) {
    uint32_t ccr = duty_cyle * (float)period;
    if (timer_channel == TIM_CHANNEL_1) {
        timer_handle.Instance->CCR1 = ccr;
    }
    if (timer_channel == TIM_CHANNEL_2) {
        timer_handle.Instance->CCR2 = ccr;
    }
    if (timer_channel == TIM_CHANNEL_3) {
        timer_handle.Instance->CCR3 = ccr;
    }
    if (timer_channel == TIM_CHANNEL_4) {
        timer_handle.Instance->CCR4 = ccr;
    }
    if (timer_channel == TIM_CHANNEL_5) {
        timer_handle.Instance->CCR5 = ccr;
    }
    if (timer_channel == TIM_CHANNEL_6) {
        timer_handle.Instance->CCR6 = ccr;
    }
}

void Pwm_output::start() {
    HAL_TIM_PWM_Start(&timer_handle, timer_channel);
}

extern "C" {
    void EXTI0_IRQHandler(void)
    {
      HAL_GPIO_EXTI_IRQHandler(gpio_interrupt_pin[0]);
    }

    void EXTI1_IRQHandler(void)
    {
      HAL_GPIO_EXTI_IRQHandler(gpio_interrupt_pin[1]);
    }

    void EXTI2_IRQHandler(void)
    {
      HAL_GPIO_EXTI_IRQHandler(gpio_interrupt_pin[2]);
    }

    void EXTI3_IRQHandler(void)
    {
      HAL_GPIO_EXTI_IRQHandler(gpio_interrupt_pin[3]);
    }

    void EXTI4_IRQHandler(void)
    {
      HAL_GPIO_EXTI_IRQHandler(gpio_interrupt_pin[4]);
    }

    void EXTI9_5_IRQHandler(void)
    {
      HAL_GPIO_EXTI_IRQHandler(gpio_interrupt_pin[5]);
    }

    void EXTI15_10_IRQHandler(void)
    {
      HAL_GPIO_EXTI_IRQHandler(gpio_interrupt_pin[6]);
    }
}
