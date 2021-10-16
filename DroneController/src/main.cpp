﻿#include "init.hpp"

uint32_t adc_input_value = 0;

int main (void) {

    init_all();
    while (1) {
        mpu6050.get_filtered_angles(0.9996);
        pid_pitch.process_pid(0, mpu6050.pitch);
        pid_roll.process_pid(0, mpu6050.roll);
        if (mpu6050.is_threshold_reached || esp32.emergency_request_received) {
            commands.throttle = 0;
            drone_state.throttle = 0;
        }

        //motor_FR.set_speed(commands.throttle);
        //motor_BR.set_speed(commands.throttle);
        //motor_FL.set_speed(commands.throttle);
        //motor_BL.set_speed(commands.throttle);

        motor_FR.set_speed(commands.throttle, -pid_roll.pid_result, pid_pitch.pid_result);
        motor_BR.set_speed(commands.throttle, pid_roll.pid_result, pid_pitch.pid_result);
        motor_FL.set_speed(commands.throttle, -pid_roll.pid_result, -pid_pitch.pid_result);
        motor_BL.set_speed(commands.throttle, pid_roll.pid_result, -pid_pitch.pid_result);


        //mpu6050.display();
        //print_commands();
    }
    // Return 0 to satisfy compiler
    return 0;
}

extern "C" {
    void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
        if (huart->Instance == (USART_TypeDef *)esp32.uart_com.instance) {
            if (esp32.process_receive()) {
                if (esp32.is_configuration_received()) {
                    if (esp32.get_PID_configuration()) {
                        pid_pitch.set_pid_values(pid_pitch_configuration.kp, pid_pitch_configuration.ki, pid_pitch_configuration.kd);
                        pid_roll.set_pid_values(pid_roll_configuration.kp, pid_roll_configuration.ki, pid_roll_configuration.kd);
                        pid_yaw.set_pid_values(pid_yaw_configuration.kp, pid_yaw_configuration.ki, pid_yaw_configuration.kd);
                        esp32.emergency_request_received = false;
                        mpu6050.is_threshold_reached = false;
                        esp32.send_ack(esp32.DronePIDConfiguration);
                    }
                }
                if (not esp32.is_emergency_request_received() && not mpu6050.is_threshold_reached) {
                    if (esp32.is_state_request_received()) {
                        esp32.send_current_state();
                    }
                    if (esp32.are_drone_commands_received()) {
                        esp32.get_drone_received_commands();
                        drone_state.pitch = mpu6050.pitch;
                        drone_state.roll = mpu6050.roll;
                        drone_state.yaw = mpu6050.yaw;
                        drone_state.throttle = commands.throttle;
                        esp32.send_current_state();
                    }
                }
                if (esp32.is_emergency_request_received()) {
                    esp32.send_ack(esp32.EmergencyRequest);
                }
                if (mpu6050.is_threshold_reached) {
                    esp32.send_thresholds_reached_error();
                }
            }
            esp32.it_start_reading();
        }
    }

    /*void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
        if (GPIO_Pin == btn.pin_number) led.toggle_state();
    }*/

    /*void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {

    }*/
}

