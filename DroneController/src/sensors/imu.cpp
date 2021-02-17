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
    is_gyro_calibrated = false;
    is_accel_calibrated = false;
    is_threshold_reached = false;
    set_gyro_sens();
    set_accel_sens();
    last_time = 0;
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
    if (i2c.memory_write(MPU6050_ADR, GYRO_CONFIG_REG, GYRO_SPEED_250, 1000) == true) {
        HAL_Delay(10);
        // Configure accelerometer
        if (i2c.memory_write(MPU6050_ADR, ACCEL_CONFIG_REG, ACCEL_SPEED_2G, 1000) == true) {
            return true;
        }
    }
    return false;
}

void IMU::calibrate_gyro() {
    int16_t gyro_data[3] = {0};
    for (int16_t i = 0; i<MAX_GYRO_CALIB_SAMPLES; i++) {
        //i2c.memory_read(MPU6050_ADR, GYRO_READ_REG, 1000, received_data, sizeof(received_data));
        if (get_raw_gyro_data()) {
            gyro_data[0] += gyro.raw_x_data;
            gyro_data[1] += gyro.raw_y_data;
            gyro_data[2] += gyro.raw_z_data;
            HAL_Delay(GYRO_CALIB_DELAY);
        }
    }
    gyro.raw_x_offset = gyro_data[0]/MAX_GYRO_CALIB_SAMPLES;
    gyro.raw_y_offset = gyro_data[1]/MAX_GYRO_CALIB_SAMPLES;
    gyro.raw_z_offset = gyro_data[2]/MAX_GYRO_CALIB_SAMPLES;
    is_gyro_calibrated = true;
}

void IMU::calibrate_accel() {
    int16_t accel_data[3] = {0};
    for (int16_t i = 0; i<MAX_ACCEL_CALIB_SAMPLES; i++) {
        //i2c.memory_read(MPU6050_ADR, GYRO_READ_REG, 1000, received_data, sizeof(received_data));
        if (get_raw_accel_data()) {
            accel_data[0] += accel.raw_x_data;
            accel_data[1] += accel.raw_y_data;
            accel_data[2] += accel.raw_z_data;
            HAL_Delay(ACCEL_CALIB_DELAY);
        }
    }
    accel.raw_x_offset = accel_data[0]/MAX_ACCEL_CALIB_SAMPLES;
    accel.raw_y_offset = accel_data[1]/MAX_ACCEL_CALIB_SAMPLES;
    accel.raw_z_offset = accel_data[2]/MAX_ACCEL_CALIB_SAMPLES;
    is_accel_calibrated = true;
}

bool IMU::get_raw_gyro_data() {
    uint8_t received_data[6] = {0};
    if (i2c.memory_read(MPU6050_ADR, GYRO_READ_REG, 1000, received_data, sizeof(received_data)) == true) {
        gyro.raw_x_data = received_data[0] << 8 | received_data[1];
        gyro.raw_y_data = received_data[2] << 8 | received_data[3];
        gyro.raw_z_data = received_data[4] << 8 | received_data[5];
        return true;
    } else {
        return false;
    }
}

bool IMU::get_raw_accel_data() {
    uint8_t received_data[6] = {0};
    if (i2c.memory_read(MPU6050_ADR, ACCEL_READ_REG, 1000, received_data, sizeof(received_data)) == true) {
        accel.raw_x_data = received_data[0] << 8 | received_data[1];
        accel.raw_y_data = received_data[2] << 8 | received_data[3];
        accel.raw_z_data = received_data[4] << 8 | received_data[5];
        return true;
    } else {
        return false;
    }
}

bool IMU::get_acceleration() {
    if (get_raw_accel_data()) {
        accel.x_accel = (float)(accel.raw_x_data - accel.raw_x_offset)/accel_sens;
        accel.y_accel = (float)(accel.raw_y_data - accel.raw_y_offset)/accel_sens;
        accel.z_accel = (float)(accel.raw_z_data - accel.raw_z_offset)/accel_sens;
        return true;
    } else {
        return false;
    }
}

bool IMU::get_gyro_angular_speed() {
    if (get_raw_gyro_data()) {
        gyro.x_angular_speed = (float)(gyro.raw_x_data - gyro.raw_x_offset)/gyro_sens;
        gyro.y_angular_speed = (float)(gyro.raw_y_data - gyro.raw_y_offset)/gyro_sens;
        gyro.z_angular_speed = (float)(gyro.raw_z_data - gyro.raw_z_offset)/gyro_sens;
        return true;
    } else {
        return false;
    }
}

bool IMU::get_raw_gyro_angles() {
    if (get_raw_gyro_data()) {
        uint32_t time_now = HAL_GetTick();
        float dT = (float)(time_now - last_time)/1000.0;
        last_time = time_now;
        gyro.raw_roll = ((float)(gyro.raw_x_data - gyro.raw_x_offset))*dT/gyro_sens + roll;
        gyro.raw_pitch = ((float)(gyro.raw_y_data - gyro.raw_y_offset))*dT/gyro_sens + pitch;
        gyro.raw_yaw = ((float)(gyro.raw_z_data - gyro.raw_z_offset))*dT/gyro_sens + yaw;
        roll = gyro.raw_roll;
        pitch = gyro.raw_pitch;
        yaw = gyro.raw_yaw;
        return true;
    } else {
        return false;
    }
}

bool IMU::get_raw_accel_angles() {
    if (get_raw_accel_data()) {
        accel.raw_roll = atan(accel.raw_y_data/sqrt(pow(accel.raw_z_data,2) + pow(accel.raw_x_data,2)))*180/M_PI;
        accel.raw_pitch = atan(-accel.raw_x_data/sqrt(pow(accel.raw_z_data, 2) + pow(accel.raw_y_data, 2)))*180/M_PI;
        return true;
    } else {
        return false;
    }

}

bool IMU::get_filtered_angles(float filtering_rate) {
    if (get_raw_accel_angles()) {
        if (get_raw_gyro_angles()) {
            roll = filtering_rate * gyro.raw_roll + (1.0 - filtering_rate) * accel.raw_roll;
            pitch = filtering_rate * gyro.raw_pitch + (1.0 - filtering_rate) * accel.raw_pitch;
            yaw = 0.0;
            if (pitch > MAX_PITCH_THRESHOLD || pitch < MIN_PITCH_THRESHOLD) {
                is_threshold_reached = true;
            }
            if (roll > MAX_ROLL_THRESHOLD || roll < MIN_ROLL_THRESHOLD) {
                is_threshold_reached = true;
            }
            return true;
        }
    }
    return false;
}

void IMU::display() {
    std::string s;
    s.append(std::to_string((int32_t)roll));
    s.append(" | ");
    s.append(std::to_string((int32_t)pitch));
    s.append(" | ");
    s.append(std::to_string((int32_t)yaw));
    uart_debug.print(s.c_str());
}

