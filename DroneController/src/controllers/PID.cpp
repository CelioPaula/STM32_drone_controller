#include "PID.hpp"
#include "maths/math.hpp"

PID::PID() {
    error = 0;
    is_constrained = false;
    previous_error = 0;
    kp_result = ki_result = kd_result = pid_result = 0;
    last_time = 0;
}

void PID::set_pid_values(float kp, float ki, float kd) {
    PID::kp = kp;
    PID::ki = ki;
    PID::kd = kd;
}

void PID::constrain(float min_constrain, float max_constrain) {
    is_constrained = true;
    PID::min_constrain = min_constrain;
    PID::max_constrain = max_constrain;
}

void PID::constrain() {
    if (is_constrained) {
        if (pid_result > max_constrain) {
            pid_result = max_constrain;
        }
        if (pid_result < min_constrain) {
            pid_result = min_constrain;
        }
    }
}

void PID::process_p(float target_value, float current_value) {
    error = (current_value - target_value);
    kp_result = kp * error;
}

void PID::process_i(float target_value, float current_value) {
    error = (current_value - target_value);
    if (error <= 3 && error >= -3) {
        ki_result = ki_result + (ki * error);
    }
}

void PID::process_d(float target_value, float current_value) {
    float time_now = HAL_GetTick();
    float elapsed_time = (float)(time_now - last_time)/1000.0;
    last_time = time_now;

    error = (current_value - target_value);
    kd_result = kd * ((error - previous_error)/elapsed_time);
    previous_error = error;
}

void PID::process_pid(float target_value, float current_value) {

    process_p(target_value, current_value);
    process_i(target_value, current_value);
    process_d(target_value, current_value);

    pid_result = kp_result + ki_result + kd_result;

    if (is_constrained) {
        constrain();
    }
}


