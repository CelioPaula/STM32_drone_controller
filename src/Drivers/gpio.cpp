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
            HAL_NVIC_SetPriority(EXTI2_IRQn, preempt_priority, sub_priority);
            HAL_NVIC_EnableIRQ(EXTI2_IRQn);
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


Pwm_output::Pwm_output(GPIO_TypeDef *gpio_port, uint16_t pin_number, uint16_t timer_period, uint16_t timer_prescaler):
    Gpio_output(gpio_port, pin_number, AlternatePushPull, SpeedUpTo_2MHz),
    timer(Timer(timer_period, timer_prescaler, gpio_port, pin_number)) {
    desired_frequency = (H_CLOCK_FREQ)/(timer_prescaler * timer_period);
}

void Pwm_output::set_alternate_function() {
    if (timer.timer_instance == TIM1) {
        gpio_init_struct.Alternate = GPIO_AF1_TIM1;
    }
    if (timer.timer_instance == TIM2) {
        gpio_init_struct.Alternate = GPIO_AF1_TIM2;
    }
    if (timer.timer_instance == TIM3) {
        gpio_init_struct.Alternate = GPIO_AF2_TIM3;
    }
    if (timer.timer_instance == TIM4) {
        gpio_init_struct.Alternate = GPIO_AF2_TIM4;
    }
    if (timer.timer_instance == TIM5) {
        gpio_init_struct.Alternate = GPIO_AF2_TIM5;
    }
}

void Pwm_output::init() {
    timer.init();
    set_alternate_function();
    Gpio_output::init();
}


void Pwm_output::set_duty_cycle(float duty_cyle) {
    uint32_t ccr = duty_cyle * (float)timer.period;
    if (timer.channel == TIM_CHANNEL_1) {
        timer.timer_handler.Instance->CCR1 = ccr;
    }
    if (timer.channel == TIM_CHANNEL_2) {
        timer.timer_handler.Instance->CCR2 = ccr;
    }
    if (timer.channel == TIM_CHANNEL_3) {
        timer.timer_handler.Instance->CCR3 = ccr;
    }
    if (timer.channel == TIM_CHANNEL_4) {
        timer.timer_handler.Instance->CCR4 = ccr;
    }
}

void Pwm_output::start() {
    HAL_TIM_PWM_Start(&timer.timer_handler, timer.channel);
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
