#pragma once
#include "stm32f4xx_hal.h"
#include "Drivers/i2c.hpp"

typedef enum {
    GyroSpeed_250,
    GyroSpeed_500,
    GyroSpeed_1000,
    GyroSpeed_2000,
}Gyro_Speed;

typedef enum {
    AccelSpeed_2G,
    AccelSpeed_4G,
    AccelSpeed_8G,
    AccelSpeed_16G,
}Accel_Speed;


class IMU
{
    public:

        IMU(GPIO_TypeDef *sda_port, uint16_t sda_pin_number, GPIO_TypeDef *scl_port, uint16_t scl_pin_number, Gyro_Speed gyro_speed, Accel_Speed accel_speed);

        bool init();

        bool configure();

        void calibrate_gyro();

        void calibrate_accel();

        bool get_raw_accel_data();

        bool get_raw_gyro_data();

        bool get_acceleration();

        bool get_gyro_angular_speed();

        bool get_raw_gyro_angles();

        bool get_raw_accel_angles();

        bool get_filtered_angles(float filtering_rate);

        void display();

        typedef struct {
            int16_t raw_x_data;
            int16_t raw_y_data;
            int16_t raw_z_data;

            int16_t raw_x_offset;
            int16_t raw_y_offset;
            int16_t raw_z_offset;

            float x_angular_speed;
            float y_angular_speed;
            float z_angular_speed;

            float raw_pitch;
            float raw_roll;
            float raw_yaw;
        }Gyro;

        typedef struct {
            int16_t raw_x_data;
            int16_t raw_y_data;
            int16_t raw_z_data;

            int16_t raw_x_offset;
            int16_t raw_y_offset;
            int16_t raw_z_offset;

            int16_t x_accel;
            int16_t y_accel;
            int16_t z_accel;

            float raw_pitch;
            float raw_roll;
        }Accel;

        Gyro gyro;
        Accel accel;

        float pitch;
        float roll;
        float yaw;

        bool is_gyro_calibrated;
        bool is_accel_calibrated;
        bool is_threshold_reached;

        uint32_t last_time;

    private:

        void set_gyro_sens();

        void set_accel_sens();

        const uint8_t MPU6050_ADR = 0xD0;

        const uint8_t WHO_AM_I_REG = 0x75;
        const uint8_t WHO_AM_I_EXPECTED = 0x68;

        const uint8_t PWR_MGMT_1_REG = 0x6B;
        const uint8_t PWR_MGMT_WAKE_UP = 0x00;

        const uint8_t GYRO_CONFIG_REG = 0x1B;
        const uint8_t GYRO_SPEED_250 = 0x00;
        const uint8_t GYRO_SPEED_500 = 0x08;
        const uint8_t GYRO_SPEED_1000 = 0x10;
        const uint8_t GYRO_SPEED_2000 = 0x18;
        const uint8_t GYRO_READ_REG = 0x43;
        const float gyro_sens_250 = 131;
        const float gyro_sens_500 = 65.5;
        const float gyro_sens_1000 = 32.8;
        const float gyro_sens_2000 = 16.4;


        const uint8_t ACCEL_CONFIG_REG = 0x1C;
        const uint8_t ACCEL_SPEED_2G = 0x00;
        const uint8_t ACCEL_SPEED_4G = 0x08;
        const uint8_t ACCEL_SPEED_8G = 0x10;
        const uint8_t ACCEL_SPEED_16G = 0x18;
        const uint8_t ACCEL_READ_REG = 0x3B;
        const int accel_sens_2G = 16384;
        const int accel_sens_4G = 8192;
        const int accel_sens_8G = 4096;
        const int accel_sens_16G = 2048;

        float gyro_sens;
        float accel_sens;

        Gyro_Speed gyro_speed;
        Accel_Speed accel_speed;

        I2C i2c;

        const uint16_t MAX_GYRO_CALIB_SAMPLES = 10;
        const uint16_t MAX_ACCEL_CALIB_SAMPLES = 10;
        const uint32_t GYRO_CALIB_DELAY = 100;
        const uint32_t ACCEL_CALIB_DELAY = 100;

        const float MAX_PITCH_THRESHOLD = 30.0;
        const float MIN_PITCH_THRESHOLD = -30.0;
        const float MAX_ROLL_THRESHOLD = 30.0;
        const float MIN_ROLL_THRESHOLD = -30.0;
};
