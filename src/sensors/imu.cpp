#include "imu.hpp"
#include "math.h"
#include "Drivers/uart.hpp"

IMU::IMU(GPIO_TypeDef *sda_port, uint16_t sda_pin_number, GPIO_TypeDef *scl_port, uint16_t scl_pin_number, Gyro_Speed gyro_speed, Accel_Speed accel_speed):
    gyro_speed(gyro_speed),
    accel_speed(accel_speed),
    i2c(I2C(sda_port, sda_pin_number, scl_port, scl_pin_number, 100000, I2C_7Bit_Addressing)) {
    gyro = {0};
    accel = {0};
    pitch = roll = yaw = 0;
    is_ready = false;
    set_gyro_sens();
    set_accel_sens();
}

void IMU::set_gyro_sens() {
    if (gyro_speed == GyroSpeed_250) {
        gyro_sens = gyro_sens_250;
    }
    if (gyro_speed == GyroSpeed_500) {
        gyro_sens = gyro_sens_500;
    }
    if (gyro_speed == GyroSpeed_1000) {
        gyro_sens = gyro_sens_1000;
    }
    if (gyro_speed == GyroSpeed_2000) {
        gyro_sens = gyro_sens_2000;
    }
}

void IMU::set_accel_sens() {
    if (accel_speed == AccelSpeed_2G) {
        accel_sens = accel_sens_2G;
    }
    if (accel_speed == AccelSpeed_4G) {
        accel_sens = accel_sens_4G;
    }
    if (accel_speed == AccelSpeed_8G) {
        accel_sens = accel_sens_8G;
    }
    if (accel_speed == AccelSpeed_16G) {
        accel_sens = accel_sens_16G;
    }
}

bool IMU::init() {
    uint8_t received_data[1] = {0};
    i2c.init();
    if (i2c.memory_read(MPU6050_ADR, WHO_AM_I_REG, 1000, received_data, sizeof(received_data)) == true) {
        if (*received_data == WHO_AM_I_EXPECTED) {
            if (i2c.memory_write(MPU6050_ADR, PWR_MGMT_1_REG, PWR_MGMT_WAKE_UP, 1000) == true) {
                return true;
            }
        }
    }
    return false;
}

bool IMU::configure() {
    // Configure gyroscope
    if (i2c.memory_write(MPU6050_ADR, GYRO_CONFIG_REG, GYRO_SPEED_500, 1000) == true) {
        HAL_Delay(10);
        // Configure accelerometer
        if (i2c.memory_write(MPU6050_ADR, ACCEL_CONFIG_REG, ACCEL_SPEED_4G, 1000) == true) {
            return true;
        }
    }
    return false;
}

void IMU::calibrate_gyro(int16_t iterations) {
    uint8_t received_data[6] = {0};
    int16_t gyro_data[3] = {0};
    for (int16_t i = 0; i<iterations; i++) {
        i2c.memory_read(MPU6050_ADR, GYRO_READ_REG, 1000, received_data, sizeof(received_data));
        gyro_data[0] += (received_data[0] << 8 | received_data[1]);
        gyro_data[1] += (received_data[2] << 8 | received_data[3]);
        gyro_data[2] += (received_data[4] << 8 | received_data[5]);
        HAL_Delay(100);
    }
    gyro.x_offset = (float)(gyro_data[0]/(iterations));
    gyro.y_offset = (float)(gyro_data[1]/(iterations));
    gyro.z_offset = (float)(gyro_data[2]/(iterations));
    is_ready = true;
}

bool IMU::get_acceleration() {
    uint8_t received_data[6] = {0};
    if (i2c.memory_read(MPU6050_ADR, ACCEL_READ_REG, 1000, received_data, sizeof(received_data)) == true) {
        accel.x_accel = (received_data[0] << 8 | received_data[1])/accel_sens;
        accel.y_accel = (received_data[2] << 8 | received_data[3])/accel_sens;
        accel.z_accel = (received_data[4] << 8 | received_data[5])/accel_sens;
        return true;
    } else {
        return false;
    }
}

bool IMU::get_angular_speed() {
    uint8_t received_data[6] = {0};
    int16_t gyro_data[3] = {0};
    if (i2c.memory_read(MPU6050_ADR, GYRO_READ_REG, 1000, received_data, sizeof(received_data)) == true) {
        gyro_data[0] = received_data[0] << 8 | received_data[1];
        gyro_data[1] = received_data[2] << 8 | received_data[3];
        gyro_data[2] = received_data[4] << 8 | received_data[5];

        gyro.x_angular_speed = (float)gyro_data[0];
        gyro.y_angular_speed = (float)gyro_data[1];
        gyro.z_angular_speed = (float)gyro_data[2];
        return true;
    } else {
        return false;
    }
}

bool IMU::get_raw_gyro_angles(float timer_period) {
    if (get_angular_speed()) {
        gyro.raw_roll = ((float)(gyro.x_angular_speed - gyro.x_offset))*timer_period/gyro_sens + (float)gyro.raw_roll;
        gyro.raw_pitch = ((float)(gyro.y_angular_speed - gyro.y_offset))*timer_period/gyro_sens + (float)gyro.raw_pitch;
        gyro.raw_yaw = ((float)(gyro.z_angular_speed - gyro.z_offset))*timer_period/gyro_sens + (float)gyro.raw_yaw;
        return true;
    } else {
        return false;
    }

}

bool IMU::get_raw_accel_angles() {
    if (get_acceleration()) {
        accel.raw_roll = atan(accel.y_accel/sqrt(pow(accel.z_accel,2) + pow(accel.x_accel,2)))*180/M_PI;
        accel.raw_pitch = atan(-accel.x_accel/sqrt(pow(accel.z_accel, 2) + pow(accel.y_accel, 2)))*180/M_PI;
        return true;
    } else {
        return false;
    }

}

bool IMU::get_filtered_angles(float timer_period, float filtering_rate) {
    if (get_raw_accel_angles()) {
        if (get_raw_gyro_angles(timer_period)) {
            roll = filtering_rate * gyro.raw_roll + (1.0 - filtering_rate) * accel.raw_roll;
            pitch = filtering_rate * gyro.raw_pitch + (1.0 - filtering_rate) * accel.raw_pitch;
            yaw = gyro.raw_yaw;
            return true;
        }
    }{}
    return false;
}

void IMU::display() {
    std::string s;
    s.append(std::to_string((int32_t)roll));
    s.append(" | ");
    s.append(std::to_string((int32_t)pitch));
    s.append(" | ");
    s.append(std::to_string((int32_t)yaw));
    uart_debug.write(s.c_str());
}

