#include "stm32f3xx_hal.h"
#include "Drivers/clock.hpp"
#include "Drivers/gpio.hpp"
#include "Drivers/uart.hpp"
#include "Drivers/adc.hpp"
#include "Drivers/dma.hpp"
#include "maths/math.hpp"

// TODO : Change adc clock

int main (void) {

    HAL_Init();

    SystemClock_Config();

    uart.default_init(38400);

    int time_high = 0;

    Adc adc_input = Adc(GPIOA, GPIO_PIN_1, ADC_Resolution_10B, false);
    //Gpio_output led = Gpio_output(GPIOA, GPIO_PIN_5, Push_Pull, SpeedUpTo_2MHz);
    //Gpio_input btn = Gpio_input(GPIOC, GPIO_PIN_4, EXTIT_RisingEdge, Pull_Up);

    //led.init();
    //btn.init();
    //btn.set_interrupt_priority(0, 0);

    adc_input.init();
    //adc_input.init_adc_dma(DMA_Priority_Low, 0, 0);

    adc_input.start_adc();

    // SERVO PWM 50Hz (5% to 10% duty cycle)
    // SERVO HS 500Hz (TO CHECK)
    Pwm_output bmotor_1 = Pwm_output(GPIOA, GPIO_PIN_6, 100, 14300);

    bmotor_1.init();
    bmotor_1.start();

    std::string s;
    float duty_cycle = 0.0;
    while (1) {
        time_high = map((long)adc_input.get_adc_value(), 0, 1023, 1000, 2000);
        duty_cycle = (float)((float)time_high/20000);
        //duty_cycle = 0.06;
        bmotor_1.set_duty_cycle(duty_cycle);
        s = "ADC Value : ";
        s.append(std::to_string(duty_cycle));
        uart.write(s.c_str());
        //HAL_Delay(100);
    }

    // Return 0 to satisfy compiler
    return 0;
}

extern "C" {
    /*void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
        if (GPIO_Pin == btn.pin_number) led.toggle_state();
    }*/

    /*void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
        std::string s = "HELLOOOOOOOOOOO";
        uart.write(s.c_str());
        __NOP();
    }*/
}

