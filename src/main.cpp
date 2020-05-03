#include "stm32f3xx_hal.h"
#include "Drivers/clock.hpp"
#include "Drivers/gpio.hpp"

Gpio_output led = Gpio_output(GPIOA, GPIO_PIN_5, Push_Pull, SpeedUpTo_2MHz);
Gpio_input btn = Gpio_input(GPIOC, GPIO_PIN_4, EXTIT_RisingEdge, Pull_Up);

int main (void) {

    HAL_Init();

    SystemClock_Config();

    led.init();
    btn.init();
    btn.set_interrupt_priority(0, 0);

    while (1) {

    }

    // Return 0 to satisfy compiler
    return 0;
}

extern "C" {
    void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
        if (GPIO_Pin == btn.pin_number) led.toggle_state();
    }
}

