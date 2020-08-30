#include "actuators/brushless.hpp"
#include "maths/math.hpp"

Brushless::Brushless(GPIO_TypeDef *gpio_port, uint16_t pin_number, uint32_t max_speed_value) :
    pwm_output(Pwm_output(gpio_port, pin_number, 1000, 1430)) {
    speed_value = 0;
    Brushless::max_speed_value = max_speed_value;
}

void Brushless::init() {
    pwm_output.init();
    pwm_output.start();
}

void Brushless::set_speed(uint32_t speed_value) {
    Brushless::speed_value = speed_value;
    uint32_t pwm_time_high = map((long)speed_value, 0, max_speed_value, min_pwm_time_high, pwm_period);
    float duty_cycle = (float)((float)pwm_time_high/(pwm_period*10));
    pwm_output.set_duty_cycle(duty_cycle);
}
