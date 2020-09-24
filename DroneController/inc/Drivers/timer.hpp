#pragma once
#include "stm32f4xx_hal.h"
#include "Drivers/clock.hpp"

typedef enum {
    TimerUsed,
    TimerUsedWithPWM,
    TimerNotUsed,
}UsedStatement;

typedef struct {
    TIM_TypeDef *timer_instance;
    UsedStatement timer_type;
}UsedTimer;

#define MAX_TIMER_INSTANCES 6

extern TIM_TypeDef *availables_timer_instances[MAX_TIMER_INSTANCES];
extern UsedTimer used_timers[MAX_TIMER_INSTANCES];
extern TIM_HandleTypeDef interrupt_timers[MAX_TIMER_INSTANCES];

void init_used_timers_array();

class Timer
{
    public:

        Timer(uint16_t period, uint16_t prescaler);

        Timer(uint16_t period, uint16_t prescaler, GPIO_TypeDef *gpio_port, uint16_t pin_number);

        void init();

        bool start_timer_interruption(uint32_t preempt_priority, uint32_t sub_priority);

        TIM_HandleTypeDef timer_handler = {0};
        TIM_TypeDef *timer_instance;
        uint32_t channel;

        uint32_t desired_frequency;

        uint16_t period;
        uint16_t prescaler;

    private:

        bool set_timer_instance();

        void set_pwm_timer_instance(GPIO_TypeDef *gpio_port, uint16_t pin_number);

        bool init_timer_struct();

        bool init_timer_clock_source_config();

        bool init_timer_master_config();

        bool init_timer_output_compare();

        void set_interrupt_priority(uint32_t preempt_priority, uint32_t sub_priority);

        bool is_pwm_timer;

        GPIO_TypeDef* gpio_port;

        uint16_t pin_number;
};
