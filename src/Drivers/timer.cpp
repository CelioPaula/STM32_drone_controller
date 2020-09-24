#include "timer.hpp"

//TIM_HandleTypeDef Timer::timer_handler = {0};
TIM_TypeDef *availables_timer_instances[MAX_TIMER_INSTANCES] = {TIM2, TIM3, TIM4, TIM5, TIM9, TIM10};
UsedTimer used_timers[MAX_TIMER_INSTANCES];

TIM_HandleTypeDef interrupt_timers[MAX_TIMER_INSTANCES] = {0};

Timer::Timer(uint16_t period, uint16_t prescaler):
    period(period),
    prescaler(prescaler) {
    is_pwm_timer = false;
    desired_frequency = (H_CLOCK_FREQ)/(prescaler * period);
}

Timer::Timer(uint16_t period, uint16_t prescaler, GPIO_TypeDef *gpio_port, uint16_t pin_number):
    period(period),
    prescaler(prescaler),
    gpio_port(gpio_port), pin_number(pin_number) {
    is_pwm_timer = true;
    channel = 0;
    desired_frequency = (H_CLOCK_FREQ)/(prescaler * period);
}

void init_used_timers_array() {
    for (uint8_t i = 0; i<MAX_TIMER_INSTANCES; i++) {
        used_timers[i].timer_instance = availables_timer_instances[i];
        used_timers[i].timer_type = TimerNotUsed;
    }
}

void Timer::init() {
    if (is_pwm_timer) {
        set_timer_instance();
        timer_clock_init(timer_instance);
        init_timer_struct();
        init_timer_clock_source_config();
        if (is_pwm_timer) {
            HAL_TIM_PWM_Init(&timer_handler);
        }
        init_timer_master_config();
        if (is_pwm_timer) {
            init_timer_output_compare();
        }
    } else {
        set_timer_instance();
        timer_clock_init(timer_instance);
        init_timer_struct();
        init_timer_clock_source_config();
        init_timer_master_config();

    }
}

bool Timer::set_timer_instance() {
    if (not is_pwm_timer) {
        for (uint8_t i = 0; i < MAX_TIMER_INSTANCES; i++) {
            if (used_timers[i].timer_type == TimerNotUsed) {
                timer_instance = used_timers[i].timer_instance;
                used_timers[i].timer_type = TimerUsed;
                return true;
            }
        }
        return false;
    } else {
        set_pwm_timer_instance(gpio_port, pin_number);
        for (uint8_t i = 0; i < MAX_TIMER_INSTANCES; i++) {
            if (timer_instance == used_timers[i].timer_instance){
                if (used_timers[i].timer_type == TimerNotUsed) {
                    used_timers[i].timer_type = TimerUsedWithPWM;
                    return true;
                }
                if(used_timers[i].timer_type == TimerUsedWithPWM) {
                    return true;
                }
                if(used_timers[i].timer_type == TimerUsed) {
                    timer_instance = NULL;
                    return false;
                }
            }
        }
        return true;
    }
}

void Timer::set_pwm_timer_instance(GPIO_TypeDef *gpio_port, uint16_t pin_number) {
    if (gpio_port == GPIOA) {
        if (pin_number == GPIO_PIN_0) {
            timer_instance = TIM5;
            channel = TIM_CHANNEL_1;
        }
        if (pin_number == GPIO_PIN_1) {
            timer_instance = TIM5;
            channel = TIM_CHANNEL_2;
        }
        if (pin_number == GPIO_PIN_2) {
            timer_instance = TIM5;
            channel = TIM_CHANNEL_3;
        }
        if (pin_number == GPIO_PIN_3) {
            timer_instance = TIM2;
            channel = TIM_CHANNEL_4;
        }
        if (pin_number == GPIO_PIN_6) {
            timer_instance = TIM3;
            channel = TIM_CHANNEL_1;
        }
        if (pin_number == GPIO_PIN_7) {
            timer_instance = TIM3;
            channel = TIM_CHANNEL_2;
        }
        if (pin_number == GPIO_PIN_11) {
            timer_instance = TIM1;
            channel = TIM_CHANNEL_4;
        }
    }
    if (gpio_port == GPIOB) {
        if (pin_number == GPIO_PIN_0) {
            timer_instance = TIM3;
            channel = TIM_CHANNEL_3;
        }
        if (pin_number == GPIO_PIN_1) {
            timer_instance = TIM3;
            channel = TIM_CHANNEL_4;
        }
        if (pin_number == GPIO_PIN_3) {
            timer_instance = TIM2;
            channel = TIM_CHANNEL_2;
        }
        if (pin_number == GPIO_PIN_10) {
            timer_instance = TIM2;
            channel = TIM_CHANNEL_3;
        }
        if (pin_number == GPIO_PIN_9) {
            timer_instance = TIM4;
            channel = TIM_CHANNEL_4;
        }
        if (pin_number == GPIO_PIN_8) {
            timer_instance = TIM4;
            channel = TIM_CHANNEL_3;
        }
        if (pin_number == GPIO_PIN_7) {
            timer_instance = TIM4;
            channel = TIM_CHANNEL_2;
        }
        if (pin_number == GPIO_PIN_6) {
            timer_instance = TIM4;
            channel = TIM_CHANNEL_1;
        }
    }
}

bool Timer::init_timer_struct() {
    /** TIMER FREQUENCY = (HCLOCK FREQ)/[(PERIOD + 1)x(PRESCALER + 1)] **/
    /** Default HCLOCK FREQ : 84 MHz **/
    timer_handler.Instance = timer_instance;
    // TIMER PRESCALER : PR
    timer_handler.Init.Prescaler = prescaler;
    timer_handler.Init.CounterMode = TIM_COUNTERMODE_UP;
    // TIMER AUTORELOAD : ARR
    timer_handler.Init.Period = period;
    timer_handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    timer_handler.Init.RepetitionCounter = 0;
    timer_handler.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&timer_handler) == HAL_OK) {
        return true;
    } else {
        return false;
    }
}

bool Timer::init_timer_clock_source_config() {
    TIM_ClockConfigTypeDef clock_source_config = {0};
    clock_source_config.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&timer_handler, &clock_source_config) == HAL_OK) {
        return true;
    } else {
        return false;
    }
}

bool Timer::init_timer_master_config() {
    TIM_MasterConfigTypeDef master_config = {0};
    master_config.MasterOutputTrigger = TIM_TRGO_RESET;
    master_config.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&timer_handler, &master_config) == HAL_OK) {
        return true;
    } else {
        return false;
    }
}

bool Timer::init_timer_output_compare() {
    TIM_OC_InitTypeDef output_compare_config = {0};
    output_compare_config.OCMode = TIM_OCMODE_PWM1;
    output_compare_config.Pulse = 0;
    output_compare_config.OCPolarity = TIM_OCPOLARITY_HIGH;
    output_compare_config.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&timer_handler, &output_compare_config, channel) == HAL_OK) {
        return true;
    } else {
        return false;
    }
}

bool Timer::start_timer_interruption(uint32_t preempt_priority, uint32_t sub_priority) {
    set_interrupt_priority(preempt_priority, sub_priority);
    if (HAL_TIM_Base_Start_IT(&timer_handler) == HAL_OK) {
        return true;
    } else {
        return false;
    }
}

void Timer::set_interrupt_priority(uint32_t preempt_priority, uint32_t sub_priority) {
    if (timer_instance == TIM2) {
        HAL_NVIC_SetPriority(TIM2_IRQn, preempt_priority, sub_priority);
        HAL_NVIC_EnableIRQ(TIM2_IRQn);
        interrupt_timers[0] = timer_handler;
    }
    if (timer_instance == TIM3) {
        HAL_NVIC_SetPriority(TIM3_IRQn, preempt_priority, sub_priority);
        HAL_NVIC_EnableIRQ(TIM3_IRQn);
        interrupt_timers[1] = timer_handler;
    }
    if (timer_instance == TIM4) {
        HAL_NVIC_SetPriority(TIM4_IRQn, preempt_priority, sub_priority);
        HAL_NVIC_EnableIRQ(TIM4_IRQn);
        interrupt_timers[2] = timer_handler;
    }
    if (timer_instance == TIM5) {
        HAL_NVIC_SetPriority(TIM5_IRQn, preempt_priority, sub_priority);
        HAL_NVIC_EnableIRQ(TIM5_IRQn);
        interrupt_timers[3] = timer_handler;
    }
    if (timer_instance == TIM9) {
        HAL_NVIC_SetPriority(TIM1_BRK_TIM9_IRQn, preempt_priority, sub_priority);
        HAL_NVIC_EnableIRQ(TIM1_BRK_TIM9_IRQn);
        interrupt_timers[4] = timer_handler;
    }
    if (timer_instance == TIM10) {
        HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn, preempt_priority, sub_priority);
        HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
        interrupt_timers[5] = timer_handler;
    }
}

extern "C" {
    void TIM2_IRQHandler(void) {
      HAL_TIM_IRQHandler(&interrupt_timers[0]);
    }
    void TIM3_IRQHandler(void) {
      HAL_TIM_IRQHandler(&interrupt_timers[1]);
    }
    void TIM4_IRQHandler(void) {
      HAL_TIM_IRQHandler(&interrupt_timers[2]);
    }
    void TIM5_IRQHandler(void) {
      HAL_TIM_IRQHandler(&interrupt_timers[3]);
    }
    void TIM1_BRK_TIM9_IRQHandler(void) {
      HAL_TIM_IRQHandler(&interrupt_timers[4]);
    }
    void TIM1_UP_TIM10_IRQHandler(void) {
      HAL_TIM_IRQHandler(&interrupt_timers[5]);
    }
}
