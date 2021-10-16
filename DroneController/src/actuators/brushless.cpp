#include "actuators/brushless.hpp"
#include "maths/math.hpp"

Brushless::Brushless(GPIO_TypeDef *gpio_port, uint16_t pin_number, uint32_t speed_offset_value) :
    pwm_output(Pwm_output(gpio_port, pin_number, TIMER_PERIOD, TIMER_PRESCALER)),
    speed_offset_value(speed_offset_value) {
    speed_value = 0;
    speed_divider = 1;
}

Brushless::Brushless(GPIO_TypeDef *gpio_port, uint16_t pin_number, uint32_t speed_offset_value, float speed_divider) :
    pwm_output(Pwm_output(gpio_port, pin_number, TIMER_PERIOD, TIMER_PRESCALER)),
    speed_offset_value(speed_offset_value) {
    speed_value = 0;
    Brushless::speed_divider = speed_divider;
}

void Brushless::init() {
    pwm_output.init();
    pwm_output.start();
}

void Brushless::set_speed(uint32_t desired_speed) {
    if (desired_speed <= MAX_SPEED_VALUE) {
        if (desired_speed != 0) {
            speed_value = desired_speed + speed_offset_value;
        } else {
            speed_value = desired_speed;
        }
        speed_value = desired_speed;
        uint32_t pwm_time_high = map((long)speed_value, 0, MAX_SPEED_VALUE, MIN_PWM_TIME_HIGH, MAX_PWM_TIME_HIGH);
        float duty_cycle = (float)((float)pwm_time_high/(PWM_PERIOD));
        pwm_output.set_duty_cycle(duty_cycle);
    }
}

void Brushless::set_speed(uint32_t desired_speed, float roll_correction, float pitch_correction) {
    if (desired_speed <= MAX_SPEED_VALUE) {
        if (desired_speed != 0) {
            speed_value = (desired_speed + roll_correction + pitch_correction)/speed_divider;
        } else {
            speed_value = desired_speed;
        }
        uint32_t pwm_time_high = map((long)speed_value, 0, MAX_SPEED_VALUE, MIN_PWM_TIME_HIGH, MAX_PWM_TIME_HIGH);
        float duty_cycle = (float)((float)pwm_time_high/(PWM_PERIOD));
        pwm_output.set_duty_cycle(duty_cycle);
    }
}
