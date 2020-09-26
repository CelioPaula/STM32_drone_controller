#include "PID.hpp"
#include "maths/math.hpp"

PID::PID() {
    error = 0;
    is_constrained = false;
    previous_error = 0;
    kp_result = ki_result = kd_result = pid_result = 0;
    use_full_pid = false;
}

void PID::set_pid_values(float kp, float ki, float kd) {
    PID::kp = kp;
    PID::ki = ki;
    PID::kd = kd;
}

void PID::constrain(int32_t min_result, int32_t max_result) {
    min_constrain = min_result;
    max_constrain = max_result;
    is_constrained = true;
}

float PID::constrain_value(float value) {
    if (is_constrained) {
        if (value > max_constrain) {
            return max_constrain;
        }
        if (value < min_constrain) {
            return min_constrain;
        }
    }
    return value;
}

void PID::process_p(float target_value, float current_value) {
    error = (current_value - target_value);
    kp_result = kp * error;
    if (not use_full_pid) {
        if (is_constrained) {
            kp_result = constrain_value(kp_result);
        }
    }
}

void PID::process_i(float target_value, float current_value) {
    error = (current_value - target_value);
    ki_result = ki_result + (ki * error);
    if (not use_full_pid) {
        if (is_constrained) {
            ki_result = constrain_value(kp_result);
        }
    }
}

void PID::process_d(float target_value, float current_value, float elapsed_time) {
    error = (current_value - target_value);
    kd_result = kd * ((error - previous_error)/elapsed_time);
    if (not use_full_pid) {
        if (is_constrained) {
            kd_result = constrain_value(kp_result);
        }
    }
}

void PID::process_pid(float target_value, float current_value, float elapsed_time) {
    use_full_pid = true;
    process_p(target_value, current_value);
    process_i(target_value, current_value);
    process_d(target_value, current_value, elapsed_time);
    pid_result = kp_result + ki_result + kd_result;
    if (is_constrained) {
        pid_result = constrain_value(pid_result);
    }
}


