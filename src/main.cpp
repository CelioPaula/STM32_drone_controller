#include "init.hpp"
#include "interfaces/global_interface.hpp"

uint32_t adc_input_value = 0;

int main (void) {

    init_all();

    std::string s = "Received : ";

    while (1) {
        //mpu6050.display();
    }

    // Return 0 to satisfy compiler
    return 0;
}

extern "C" {
    void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
        if (htim->Instance == IMU_scheduler.timer_instance) {
            //if (mpu6050.is_ready) {
                float scheduler_period = 1/(float)IMU_scheduler.desired_frequency;
                //mpu6050.get_raw_gyro_angles(scheduler_period);
                //mpu6050.get_filtered_angles(scheduler_period, 0.98);

                /*pid_roll.process_pid(scheduler_period);
                pid_pitch.process_d(scheduler_period);
                */

                //mpu6050.display();
                motor_FR.set_speed(commands.desired_throttle);
                motor_BR.set_speed(commands.desired_throttle);
                motor_FL.set_speed(commands.desired_throttle);
                motor_BL.set_speed(commands.desired_throttle);
           // }
        }
    }

    void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
        if (huart->Instance == uart_com.instance) {
            uart_com.start_reading_interrupt();
            process_messages();
        }
    }

    /*void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
        if (GPIO_Pin == btn.pin_number) led.toggle_state();
    }*/

    /*void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {

    }*/
}

