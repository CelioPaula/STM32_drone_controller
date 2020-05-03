#include "gpio.hpp"

uint16_t gpio_interrupt_pin[7] = {0};

Gpio::Gpio(GPIO_TypeDef *gpio_port, uint16_t gpio_pin):
    pin_number(gpio_pin),
    gpio_port(gpio_port),
    gpio_state(GPIO_State_LOW){

}

void Gpio::init_struct() {
    gpio_clock_init(gpio_port);

    gpio_init_struct = {0};
    gpio_init_struct.Pin = pin_number;
}

Gpio_input::Gpio_input(GPIO_TypeDef *gpio_port, uint16_t gpio_pin, Input_Mode input_mode, Pull_Mode pull_mode):
    Gpio(gpio_port, gpio_pin),
    input_mode(input_mode),
    pull_mode(pull_mode) {
}

void Gpio_input::init() {
    init_struct();

    switch (input_mode) {
        case FloatingInput :
            gpio_init_struct.Mode = GPIO_MODE_INPUT;
            break;
        case AnalogInput :
            gpio_init_struct.Mode = GPIO_MODE_ANALOG;
            break;
        case EXTIT_RisingEdge :
            gpio_init_struct.Mode = GPIO_MODE_IT_RISING;
            //SET NVIC
            break;
        case EXTIT_FallingEdge :
            gpio_init_struct.Mode = GPIO_MODE_IT_FALLING;
            //SET NVIC
            break;
        default :
            gpio_init_struct.Mode = GPIO_MODE_INPUT;
            break;
    }

    switch (pull_mode) {
        case No_Pull :
            gpio_init_struct.Pull = GPIO_NOPULL;
            break;
        case Pull_Up :
            gpio_init_struct.Pull = GPIO_PULLUP;
            break;
        case Pull_Down :
            gpio_init_struct.Pull = GPIO_PULLDOWN;
            break;
        default :
            gpio_init_struct.Pull = GPIO_NOPULL;
            break;
    }
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

Gpio_output::Gpio_output(GPIO_TypeDef *gpio_port, uint16_t gpio_pin, Output_Mode output_mode, Output_Speed output_speed):
    Gpio(gpio_port, gpio_pin),
    output_mode(output_mode),
    output_speed(output_speed){
}

void Gpio_output::init() {
    init_struct();
    switch (output_mode) {
        case Push_Pull:
            gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
            break;
        case Open_Drain:
            gpio_init_struct.Mode = GPIO_MODE_OUTPUT_OD;
            break;
        default:
            gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
            break;
    }

    switch (output_speed) {
        case SpeedUpTo_2MHz:
            gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;
            break;
        case SpeedUpTo_10MHz:
            gpio_init_struct.Speed = GPIO_SPEED_FREQ_MEDIUM;
            break;
        case SpeedUpTo_50MHz:
            gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
            break;
        default:
            gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;
            break;
    }
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
