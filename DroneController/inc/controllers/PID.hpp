#pragma once
#include "stm32f4xx_hal.h"

class PID {

    public:

        PID();

        void set_pid_values(float kp, float ki, float kd);
        void process_p(float target_value, float current_value);
        void process_i(float target_value, float current_value);
        void process_d(float target_value, float current_value);
        void process_pid(float target_value, float current_value);
        void constrain(float min_result, float max_result);
        void constrain();

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

        float last_time;

        bool is_constrained;
};

