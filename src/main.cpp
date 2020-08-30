#include "init.h"

int main (void) {

    init_all();

    Adc adc_input = Adc(GPIOA, GPIO_PIN_0, ADC_Resolution_12B, false);
    Brushless motor_1 = Brushless(GPIOA, GPIO_PIN_6, adc_input.max_converted_value);
    //Gpio_output led = Gpio_output(GPIOA, GPIO_PIN_5, Push_Pull, SpeedUpTo_2MHz);
    //Gpio_input btn = Gpio_input(GPIOC, GPIO_PIN_4, EXTIT_RisingEdge, Pull_Up);

    //led.init();
    //btn.init();
    //btn.set_interrupt_priority(0, 0);

    adc_input.init();
    //adc_input.init(DMA_Priority_Low, 0, 0);
    motor_1.init();

    adc_input.start_adc();

    uint32_t adc_input_value = 0;
    std::string s;
    while (1) {
        s = "ADC value : ";
        adc_input_value = adc_input.get_adc_value();
        motor_1.set_speed(adc_input_value);
        s.append(std::to_string(adc_input_value));
        uart.write(s.c_str());
    }

    // Return 0 to satisfy compiler
    return 0;
}

extern "C" {

    /*void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
        if (GPIO_Pin == btn.pin_number) led.toggle_state();
    }*/

    /*void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {

    }*/
}

