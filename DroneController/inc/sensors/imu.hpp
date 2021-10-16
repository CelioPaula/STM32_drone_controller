#pragma once
#include "stm32f4xx_hal.h"
#include "Drivers/i2c.hpp"

typedef enum {
    GyroSens_250,
    GyroSens_500,
    GyroSens_1000,
    GyroSens_2000,
}Gyro_Sens;

typedef enum {
    AccelSens_2G,
    AccelSens_4G,
    AccelSens_8G,
    AccelSens_16G,
}Accel_Sens;


class IMU
{
    public:

        IMU(GPIO_TypeDef *sda_port, uint16_t sda_pin_number, GPIO_TypeDef *scl_port, uint16_t scl_pin_number, Gyro_Sens gyro_sens, Accel_Sens accel_sens);

        bool init(bool use_threshold);

        bool configure();

        void calibrate_gyro();

        void calibrate_accel();

        void calibrate();

        bool get_raw_accel_data();

        bool get_raw_gyro_data();

        bool get_acceleration(bool with_error);

        bool get_gyro_angular_speed(bool with_error);

        bool get_gyro_angles();

        bool get_accel_angles();

        bool get_filtered_angles(float filtering_rate);

        void display();

        float pitch;
        float roll;
        float yaw;

        bool is_threshold_reached;

    private:

        void set_gyro_sens();

        void set_accel_sens();

        void apply_sma_filter(uint16_t n);

        Gyro_Sens gyro_sens;
        Accel_Sens accel_sens;

        I2C i2c;

        typedef struct {
            int16_t raw_x_data;
            int16_t raw_y_data;
            int16_t raw_z_data;

            float x_angular_speed_error;
            float y_angular_speed_error;
            float z_angular_speed_error;

            float x_angular_speed;
            float y_angular_speed;
            float z_angular_speed;

            float pitch;
            float roll;
            float yaw;

            float sensivity;
        }Gyro;

        typedef struct {
            int16_t raw_x_data;
            int16_t raw_y_data;
            int16_t raw_z_data;

            float x_accel_error;
            float y_accel_error;
            float z_accel_error;

            float x_accel;
            float y_accel;
            float z_accel;

            float pitch;
            float roll;

            float sensivity;
        }Accel;

        Gyro gyro;
        Accel accel;

        float last_time;

        bool use_threshold_angles;

        const static uint16_t SMA_ENTRIES = 5;

        float sma_x_fifo[SMA_ENTRIES];
        float sma_y_fifo[SMA_ENTRIES];
        float sma_z_fifo[SMA_ENTRIES];

        uint8_t accel_sens_reg_value;
        uint8_t gyro_sens_reg_value;

        const uint8_t MPU6050_ADR = 0xD0;

        const uint8_t WHO_AM_I_REG = 0x75;
        const uint8_t WHO_AM_I_EXPECTED = 0x68;

        const uint8_t PWR_MGMT_1_REG = 0x6B;
        const uint8_t PWR_MGMT_WAKE_UP = 0x00;

        const uint8_t GYRO_CONFIG_REG = 0x1B;
        const uint8_t GYRO_SENS_250_REG_VALUE = 0x00;
        const uint8_t GYRO_SENS_500_REG_VALUE = 0x08;
        const uint8_t GYRO_SENS_1000_REG_VALUE = 0x10;
        const uint8_t GYRO_SENS_2000_REG_VALUE = 0x18;
        const uint8_t GYRO_READ_REG = 0x43;

        const float GYRO_SENS_250_VALUE = 131;
        const float GYRO_SENS_500_VALUE = 65.5;
        const float GYRO_SENS_1000_VALUE = 32.8;
        const float GYRO_SENS_2000_VALUE = 16.4;


        const uint8_t ACCEL_CONFIG_REG = 0x1C;
        const uint8_t ACCEL_SENS_2G_REG_VALUE = 0x00;
        const uint8_t ACCEL_SENS_4G_REG_VALUE = 0x08;
        const uint8_t ACCEL_SENS_8G_REG_VALUE = 0x10;
        const uint8_t ACCEL_SENS_16G_REG_VALUE = 0x18;
        const uint8_t ACCEL_READ_REG = 0x3B;

        const int ACCEL_SENS_2G_VALUE = 16384;
        const int ACCEL_SENS_4G_VALUE = 8192;
        const int ACCEL_SENS_8G_VALUE = 4096;
        const int ACCEL_SENS_16G_VALUE = 2048;


        const uint16_t MAX_GYRO_CALIB_SAMPLES = 200;
        const uint16_t MAX_ACCEL_CALIB_SAMPLES = 200;
        const uint32_t GYRO_CALIB_DELAY = 10;
        const uint32_t ACCEL_CALIB_DELAY = 10;

        const float MAX_PITCH_THRESHOLD = 50.0;
        const float MIN_PITCH_THRESHOLD = -50.0;
        const float MAX_ROLL_THRESHOLD = 50.0;
        const float MIN_ROLL_THRESHOLD = -50.0;
};
