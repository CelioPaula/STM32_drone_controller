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

        void calibrate_gyro(int16_t iterations);

        bool get_acceleration();

        bool get_angular_speed();

        bool get_raw_gyro_angles(float timer_period);

        bool get_raw_accel_angles();

        bool get_filtered_angles(float timer_period, float filtering_rate);

        void display();

        typedef struct {
            float x_offset;
            float y_offset;
            float z_offset;

            float x_angular_speed;
            float y_angular_speed;
            float z_angular_speed;

            float raw_pitch;
            float raw_roll;
            float raw_yaw;
        }Gyro;

        typedef struct {
            float x_accel;
            float y_accel;
            float z_accel;

            float raw_pitch;
            float raw_roll;
        }Accel;

        Gyro gyro;
        Accel accel;

        float pitch;
        float roll;
        float yaw;

        bool is_ready;

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
};
