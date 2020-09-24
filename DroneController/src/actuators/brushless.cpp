#include "actuators/brushless.hpp"
#include "maths/math.hpp"

Brushless::Brushless(GPIO_TypeDef *gpio_port, uint16_t pin_number, uint32_t max_speed_value, uint32_t speed_offset_value) :
    pwm_output(Pwm_output(gpio_port, pin_number, timer_period, timer_prescaler)),
    max_speed_value(max_speed_value),
    speed_offset_value(speed_offset_value) {
    speed_value = 0;
    Brushless::max_speed_value = max_speed_value;
    pwm_frequency = (H_CLOCK_FREQ)/(timer_prescaler * timer_period);
}

void Brushless::init() {
    pwm_output.init();
    pwm_output.start();
}

void Brushless::set_speed(uint32_t desired_speed) {
    if (desired_speed <= max_speed_value) {
        if (desired_speed != 0) {
            speed_value = desired_speed + speed_offset_value;
        } else {
            speed_value = desired_speed;
        }
        uint32_t pwm_time_high = map((long)speed_value, 0, max_speed_value, min_pwm_time_high, pwm_period);
        float duty_cycle = (float)((float)pwm_time_high/(pwm_period*10));
        pwm_output.set_duty_cycle(duty_cycle);
    }
}

void Brushless::set_speed(uint32_t desired_speed, uint32_t roll_correction, uint32_t pitch_correction) {
    speed_value = desired_speed + speed_offset_value + roll_correction + pitch_correction;
    uint32_t pwm_time_high = map((long)speed_value, 0, max_speed_value, min_pwm_time_high, pwm_period);
    float duty_cycle = (float)((float)pwm_time_high/(pwm_period*10));
    pwm_output.set_duty_cycle(duty_cycle);
}
