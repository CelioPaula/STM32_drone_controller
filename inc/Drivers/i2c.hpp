#pragma once
#include "stm32f4xx_hal.h"
#include "Drivers/clock.hpp"


typedef enum {
    I2C_7Bit_Addressing = I2C_ADDRESSINGMODE_7BIT,
    I2C_10Bit_Addressing = I2C_ADDRESSINGMODE_10BIT,
}I2C_AddressingMode;

class I2C {

    public:
        I2C(GPIO_TypeDef *sda_port, uint16_t sda_pin_number, GPIO_TypeDef *scl_port, uint16_t scl_pin_number, uint32_t i2c_frequency, I2C_AddressingMode addressing_mode);

        void init();

        bool memory_read(uint8_t device_address, uint8_t register_address, uint32_t timeout_duration, uint8_t *data, uint8_t data_size);

        bool memory_write(uint8_t device_address, uint8_t register_address, uint8_t data, uint32_t timeout_duration);

    private:

        void init_pinout(bool is_sda);

        void set_instance();

    protected:

        GPIO_InitTypeDef gpio_init_struct = {0};

        GPIO_TypeDef *sda_port;
        uint16_t sda_pin_number;

        GPIO_TypeDef *scl_port;
        uint16_t scl_pin_number;

        static I2C_HandleTypeDef i2c_handle;
        I2C_TypeDef *i2c_instance = {0};
};

class I2C_master : public I2C {

    public:
        I2C_master(GPIO_TypeDef *sda_port, uint16_t sda_pin_number, GPIO_TypeDef *scl_port, uint16_t scl_pin_number, uint32_t i2c_frequency, I2C_AddressingMode addressing_mode, uint8_t slave_address);

        bool transmit(uint8_t data_1, uint8_t data_2, bool send_all_data, uint32_t timeout_duration);

        bool receive(uint32_t timeout_duration);

    private:

        uint8_t slave_address;

        bool is_slave_connected;

        uint8_t i2c_rx_data[2];
};

class I2C_slave : public I2C {

    public:
        I2C_slave(GPIO_TypeDef *sda_port, uint16_t sda_pin_number, GPIO_TypeDef *scl_port, uint16_t scl_pin_number, uint32_t i2c_frequency, I2C_AddressingMode addressing_mode, uint32_t address);

    private:
        uint32_t address;
};
