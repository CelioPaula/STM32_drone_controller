#pragma once
#include "stm32f4xx_hal.h"

class PID {

    public:

        PID();

        void set_pid_values(float kp, float ki, float kd);
        void process_p(float target_value, float current_value);
        void process_i(float target_value, float current_value);
        void process_d(float target_value, float current_value, float elapsed_time);
        void process_pid(float target_value, float current_value, float elapsed_time);
        void constrain(int32_t min_result, int32_t max_result);
        float constrain_value(float value);

        float kp;
        float ki;
        float kd;

        float kp_result;
        float ki_result;
        float kd_result;
        float pid_result;

    private:

        float error;
        float min_constrain;
        float max_constrain;
        float previous_error;

        bool is_constrained;
        bool use_full_pid;
};

