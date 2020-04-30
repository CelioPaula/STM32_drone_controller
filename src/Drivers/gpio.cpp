#include "gpio.hpp"

gpio::gpio() {

}

void gpio::init() {
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

void gpio_output::set_state(GPIO_PinState pin_state) {
    HAL_GPIO_WritePin(gpio_port, pin_number, pin_state);
}
