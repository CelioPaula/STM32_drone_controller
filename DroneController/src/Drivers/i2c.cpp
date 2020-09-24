#include "i2c.hpp"

I2C_HandleTypeDef I2C::i2c_handle = {0};

I2C::I2C(GPIO_TypeDef *sda_port, uint16_t sda_pin_number, GPIO_TypeDef *scl_port, uint16_t scl_pin_number, uint32_t i2c_frequency, I2C_AddressingMode addressing_mode) :
    sda_port(sda_port), sda_pin_number(sda_pin_number),
    scl_port(scl_port), scl_pin_number(scl_pin_number) {

    set_instance();
    i2c_handle.Instance = i2c_instance;
    i2c_handle.Init.ClockSpeed = i2c_frequency;
    i2c_handle.Init.DutyCycle = I2C_DUTYCYCLE_2;
    i2c_handle.Init.OwnAddress1 = 0;
    i2c_handle.Init.AddressingMode = addressing_mode;
    i2c_handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    i2c_handle.Init.OwnAddress2 = 0;
    i2c_handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    i2c_handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
}

void I2C::init() {
    gpio_clock_init(sda_port);
    gpio_clock_init(scl_port);

    init_pinout(true);
    init_pinout(false);
    i2c_clock_init(i2c_instance);
    HAL_I2C_Init(&i2c_handle);
}

void I2C::init_pinout(bool is_sda) {
    gpio_init_struct.Mode = GPIO_MODE_AF_OD;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    if (i2c_instance == (I2C_TypeDef *)I2C1) {
        gpio_init_struct.Alternate = GPIO_AF4_I2C1;
    }
    if (i2c_instance == (I2C_TypeDef *)I2C2) {
        gpio_init_struct.Alternate = GPIO_AF4_I2C2;
    }
    if (i2c_instance == (I2C_TypeDef *)I2C3) {
        gpio_init_struct.Alternate = GPIO_AF4_I2C3;
    }
    if (is_sda) {
        gpio_init_struct.Pin = sda_pin_number;
        HAL_GPIO_Init(sda_port, &gpio_init_struct);
    } else {
        gpio_init_struct.Pin = scl_pin_number;
        HAL_GPIO_Init(scl_port, &gpio_init_struct);
    }
}

void I2C::set_instance() {
    if ((sda_port == GPIOB && sda_pin_number == GPIO_PIN_7) && (scl_port == GPIOB && scl_pin_number == GPIO_PIN_6)) {
        i2c_instance = I2C1;
    }
    // B3 already used by the JTAG pin
    if ((sda_port == GPIOB && sda_pin_number == GPIO_PIN_3) && (scl_port == GPIOB && scl_pin_number == GPIO_PIN_10)) {
        i2c_instance = I2C2;
    }
    if ((sda_port == GPIOC && sda_pin_number == GPIO_PIN_9) && (scl_port == GPIOA && scl_pin_number == GPIO_PIN_8)) {
        i2c_instance = I2C3;
    }
}

bool I2C::memory_read(uint8_t device_address, uint8_t register_address, uint32_t timeout_duration, uint8_t* data, uint8_t data_size) {
    if (HAL_I2C_Mem_Read(&i2c_handle, device_address, register_address, 1, data, data_size, timeout_duration) == HAL_OK) {
        return true;
    } else {
        return false;
    }
}

bool I2C::memory_write(uint8_t device_address, uint8_t register_address, uint8_t data, uint32_t timeout_duration) {
    if (HAL_I2C_Mem_Write(&i2c_handle, device_address, register_address, sizeof(register_address), &data, sizeof(data), timeout_duration) == HAL_OK) {
        return true;
    } else {
        return false;
    }
}


I2C_master::I2C_master(GPIO_TypeDef *sda_port, uint16_t sda_pin_number, GPIO_TypeDef *scl_port, uint16_t scl_pin_number, uint32_t i2c_frequency, I2C_AddressingMode addressing_mode, uint8_t slave_address) :
    I2C(sda_port, sda_pin_number, scl_port, scl_pin_number, i2c_frequency, addressing_mode),
    slave_address(slave_address) {

}

bool I2C_master::transmit(uint8_t data_1, uint8_t data_2, bool send_all_data, uint32_t timeout_duration) {
    uint8_t i2c_tx_data[2] = {data_1, data_2};
    if (send_all_data) {
        if (HAL_I2C_Master_Transmit(&i2c_handle, slave_address, i2c_tx_data, 2, timeout_duration) == HAL_OK) {
            return true;
        } else {
            return false;
        }
    } else {
        if (HAL_I2C_Master_Transmit(&i2c_handle, slave_address, i2c_tx_data, 1, timeout_duration) == HAL_OK) {
            return true;
        } else {
            return false;
        }
    }
}

bool I2C_master::receive(uint32_t timeout_duration) {
    if (HAL_I2C_Master_Receive(&i2c_handle, slave_address, i2c_rx_data, 2, timeout_duration) == HAL_OK) {
        return true;
    } else {
        return false;
    }
}

I2C_slave::I2C_slave(GPIO_TypeDef *sda_port, uint16_t sda_pin_number, GPIO_TypeDef *scl_port, uint16_t scl_pin_number, uint32_t i2c_frequency, I2C_AddressingMode addressing_mode, uint32_t address) :
    I2C(sda_port, sda_pin_number, scl_port, scl_pin_number, i2c_frequency, addressing_mode),
    address(address) {
    i2c_handle.Init.OwnAddress1 = address;
}
