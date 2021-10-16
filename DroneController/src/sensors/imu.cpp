#include "imu.hpp"
#include "math.hpp"
#include <string.h>
#include "Drivers/uart.hpp"

IMU::IMU(GPIO_TypeDef *sda_port, uint16_t sda_pin_number, GPIO_TypeDef *scl_port, uint16_t scl_pin_number, Gyro_Sens gyro_sens, Accel_Sens accel_sens):
    gyro_sens(gyro_sens),
    accel_sens(accel_sens),
    i2c(I2C(sda_port, sda_pin_number, scl_port, scl_pin_number, 100000, I2C_7Bit_Addressing)) {
    gyro = {0};
    accel = {0};
    pitch = roll = yaw = 0;
    is_threshold_reached = false;
    last_time = 0;
    accel_sens_reg_value = 0x00;
    gyro_sens_reg_value = 0x00;
    set_gyro_sens();
    set_accel_sens();
}

void IMU::set_gyro_sens() {
    if (gyro_sens == GyroSens_250) {
        gyro.sensivity = GYRO_SENS_250_VALUE;
        gyro_sens_reg_value = GYRO_SENS_250_REG_VALUE;
    }
    if (gyro_sens == GyroSens_500) {
        gyro.sensivity = GYRO_SENS_500_VALUE;
        gyro_sens_reg_value = GYRO_SENS_500_REG_VALUE;
    }
    if (gyro_sens == GyroSens_1000) {
        gyro.sensivity = GYRO_SENS_1000_VALUE;
        gyro_sens_reg_value = GYRO_SENS_1000_REG_VALUE;
    }
    if (gyro_sens == GyroSens_2000) {
        gyro.sensivity = GYRO_SENS_2000_VALUE;
        gyro_sens_reg_value = GYRO_SENS_2000_REG_VALUE;
    }
}

void IMU::set_accel_sens() {
    if (accel_sens == AccelSens_2G) {
        accel.sensivity = ACCEL_SENS_2G_VALUE;
        accel_sens_reg_value = ACCEL_SENS_2G_REG_VALUE;
    }
    if (accel_sens == AccelSens_4G) {
        accel.sensivity = ACCEL_SENS_4G_VALUE;
        accel_sens_reg_value = ACCEL_SENS_4G_REG_VALUE;
    }
    if (accel_sens == AccelSens_8G) {
        accel.sensivity = ACCEL_SENS_8G_VALUE;
        accel_sens_reg_value = ACCEL_SENS_8G_REG_VALUE;
    }
    if (accel_sens == AccelSens_16G) {
        accel.sensivity = ACCEL_SENS_16G_VALUE;
        accel_sens_reg_value = ACCEL_SENS_16G_REG_VALUE;
    }
}

bool IMU::init(bool use_threshold_angles) {
    uint8_t received_data[1] = {0};
    IMU::use_threshold_angles = use_threshold_angles;
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
    if (i2c.memory_write(MPU6050_ADR, GYRO_CONFIG_REG, gyro_sens_reg_value, 1000) == true) {
        HAL_Delay(10);
        // Configure accelerometer
        if (i2c.memory_write(MPU6050_ADR, ACCEL_CONFIG_REG, accel_sens_reg_value, 1000) == true) {
            return true;
        }
    }
    return false;
}

void IMU::calibrate_gyro() {
    float gyro_data[3] = {0};
    for (uint32_t i = 0; i<MAX_GYRO_CALIB_SAMPLES; i++) {
        if (get_gyro_angular_speed(true)) {
            gyro_data[0] += gyro.x_angular_speed;
            gyro_data[1] += gyro.y_angular_speed;
            gyro_data[2] += gyro.z_angular_speed;
            HAL_Delay(GYRO_CALIB_DELAY);
        }
    }
    gyro.x_angular_speed_error = gyro_data[0]/MAX_GYRO_CALIB_SAMPLES;
    gyro.y_angular_speed_error = gyro_data[1]/MAX_GYRO_CALIB_SAMPLES;
    gyro.z_angular_speed_error = gyro_data[2]/MAX_GYRO_CALIB_SAMPLES;
}

void IMU::calibrate_accel() {
    float accel_data[3] = {0};
    for (int16_t i = 0; i<MAX_ACCEL_CALIB_SAMPLES; i++) {
        if (get_acceleration(true)) {
            accel_data[0] += accel.x_accel;
            accel_data[1] += accel.y_accel;
            HAL_Delay(ACCEL_CALIB_DELAY);
        }
    }
    accel.x_accel_error = accel_data[0]/MAX_ACCEL_CALIB_SAMPLES;
    accel.y_accel_error = accel_data[1]/MAX_ACCEL_CALIB_SAMPLES;
    accel.z_accel_error = accel_data[2]/MAX_ACCEL_CALIB_SAMPLES;
}

void IMU::calibrate() {
    calibrate_accel();
    calibrate_gyro();
    while (not get_accel_angles());
    // To avoid floor inclination
    pitch = accel.pitch;
    roll = accel.roll;
}

bool IMU::get_raw_gyro_data() {
    uint8_t received_data[6] = {0};
    if (i2c.memory_read(MPU6050_ADR, GYRO_READ_REG, 100, received_data, sizeof(received_data)) == true) {
        gyro.raw_x_data = (received_data[0] << 8 | received_data[1]);
        gyro.raw_y_data = (received_data[2] << 8 | received_data[3]);
        gyro.raw_z_data = (received_data[4] << 8 | received_data[5]);
        return true;
    } else {
        return false;
    }
}

bool IMU::get_raw_accel_data() {
    uint8_t received_data[6] = {0};
    if (i2c.memory_read(MPU6050_ADR, ACCEL_READ_REG, 100, received_data, sizeof(received_data)) == true) {
        accel.raw_x_data = (received_data[0] << 8 | received_data[1]);
        accel.raw_y_data = (received_data[2] << 8 | received_data[3]);
        accel.raw_z_data = (received_data[4] << 8 | received_data[5]);
        return true;
    } else {
        return false;
    }
}

bool IMU::get_acceleration(bool with_error) {
    if (get_raw_accel_data()) {
        if (with_error) {
            accel.x_accel = (float)(accel.raw_x_data/accel.sensivity);
            accel.y_accel = (float)(accel.raw_y_data/accel.sensivity);
            accel.z_accel = (float)(accel.raw_z_data/accel.sensivity);
        } else {
            accel.x_accel = (float)(accel.raw_x_data/accel.sensivity) - accel.x_accel_error;
            accel.y_accel = (float)(accel.raw_y_data/accel.sensivity) - accel.y_accel_error;
            accel.z_accel = (float)(accel.raw_z_data/accel.sensivity) - accel.z_accel_error;
        }
        apply_sma_filter(SMA_ENTRIES);
        return true;
    } else {
        return false;
    }
}

bool IMU::get_gyro_angular_speed(bool with_error) {
    if (get_raw_gyro_data()) {
        if (with_error) {
            gyro.x_angular_speed = (float)(gyro.raw_x_data/gyro.sensivity);
            gyro.y_angular_speed = (float)(gyro.raw_y_data/gyro.sensivity);
            gyro.z_angular_speed = (float)(gyro.raw_z_data/gyro.sensivity);
        } else {
            gyro.x_angular_speed = (float)(gyro.raw_x_data/gyro.sensivity) - gyro.x_angular_speed_error;
            gyro.y_angular_speed = (float)(gyro.raw_y_data/gyro.sensivity) - gyro.y_angular_speed_error;
            gyro.z_angular_speed = (float)(gyro.raw_z_data/gyro.sensivity) - gyro.z_angular_speed_error;
        }
        return true;
    } else {
        return false;
    }
}

bool IMU::get_gyro_angles() {
    float time_now = HAL_GetTick();
    float dT = (float)(time_now - last_time)/1000.0;
    last_time = time_now;
    if (get_gyro_angular_speed(false)) {
        gyro.roll = (gyro.x_angular_speed*dT) + roll;
        gyro.pitch = (gyro.y_angular_speed*dT) + pitch;
        gyro.yaw = (gyro.z_angular_speed*dT) + yaw;
        return true;
    } else {
        return false;
    }
}

bool IMU::get_accel_angles() {
    if (get_acceleration(false)) {
            accel.roll = atan(accel.y_accel/sqrt(pow(accel.x_accel,2) + pow(accel.z_accel,2)))*180/M_PI;
            accel.pitch = atan(-1*accel.x_accel/sqrt(pow(accel.y_accel, 2) + pow(accel.z_accel, 2)))*180/M_PI;
        return true;
    } else {
        return false;
    }

}

bool IMU::get_filtered_angles(float filtering_rate) {
    if (get_accel_angles()) {
        if (get_gyro_angles()) {
            roll = filtering_rate * (gyro.roll) + (1.0 - filtering_rate) * accel.roll;
            pitch = filtering_rate * (gyro.pitch) + (1.0 - filtering_rate) * accel.pitch;
            yaw = gyro.yaw;

            //roll -= pitch * sin(gyro.raw_yaw * (M_PI/180));
            //pitch += roll * sin(gyro.raw_yaw * (M_PI/180));

            if (use_threshold_angles) {
                if (pitch > MAX_PITCH_THRESHOLD || pitch < MIN_PITCH_THRESHOLD) {
                    is_threshold_reached = true;
                }
                if (roll > MAX_ROLL_THRESHOLD || roll < MIN_ROLL_THRESHOLD) {
                    is_threshold_reached = true;
                }
            }
            return true;
        }
    }
    return false;
}

void IMU::apply_sma_filter(uint16_t n) {
    float sma_x_temp_fifo[n] = {0};
    float sma_y_temp_fifo[n] = {0};
    float sma_z_temp_fifo[n] = {0};

    memcpy(sma_x_temp_fifo, sma_x_fifo, n);
    memcpy(sma_y_temp_fifo, sma_y_fifo, n);
    memcpy(sma_z_temp_fifo, sma_z_fifo, n);

    for (uint16_t i = 0; i<n; i++) {
        if (i != n - 1) {
            sma_x_fifo[i+1] = sma_x_temp_fifo[i];
            sma_y_fifo[i+1] = sma_y_temp_fifo[i];
            sma_z_fifo[i+1] = sma_z_temp_fifo[i];
        }
    }
    sma_x_fifo[0] = accel.x_accel;
    sma_y_fifo[0] = accel.y_accel;
    sma_z_fifo[0] = accel.z_accel;

    accel.x_accel = 0;
    accel.y_accel = 0;
    accel.z_accel = 0;

    for (uint16_t i = 0; i<n; i++) {
        accel.x_accel += sma_x_fifo[i];
        accel.y_accel += sma_y_fifo[i];
        accel.z_accel += sma_z_fifo[i];
    }
    accel.x_accel = accel.x_accel/n;
    accel.y_accel = accel.y_accel/n;
    accel.z_accel = accel.z_accel/n;
}

void IMU::display() {
    std::string s;
    s.append(std::to_string(roll));
    s.append(" | ");
    s.append(std::to_string(pitch));
    s.append(" | ");
    s.append(std::to_string(yaw));
    uart_debug.print(s.c_str());
}

