#include "Drivers/uart.hpp"
#include "string.h"

Uart uart = Uart(GPIOA, GPIO_PIN_3, GPIO_PIN_2);

Uart::Uart(GPIO_TypeDef *gpio_port, uint16_t rx_pin, uint16_t tx_pin):
    gpio_port(gpio_port),
    rx_pin(rx_pin),
    tx_pin(tx_pin) {
}

void Uart::init(uint32_t baudrate) {
    set_instance();
    uart_clock_init(instance);
    gpio_clock_init(gpio_port);

    init_pinout(true);
    init_pinout(false);
    uart_handle.Init.BaudRate = baudrate;
    uart_handle.Instance = instance;
    uart_handle.Init.Mode = UART_RX_TX_Mode;
    HAL_UART_Init(&uart_handle);
}

void Uart::init_pinout(bool is_tx) {

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (is_tx) {
        GPIO_InitStruct.Pin = tx_pin;
    } else {
        GPIO_InitStruct.Pin = rx_pin;
    }
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    if (instance == USART1) {
        GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    }
    if (instance == USART2) {
        GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    }
    if (instance == USART6) {
        GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
    }
    HAL_GPIO_Init(gpio_port, &GPIO_InitStruct);
}

void Uart::default_init(uint32_t baudrate) {
    set_data_frame(UART_8B_Word_Length, UART_1_Stop_Bits, UART_Parity_None);
    set_control_flow(UART_NONE_Ctl);
    set_sampling(UART_8_Oversampling);
    init(baudrate);
}

void Uart::set_data_frame(UART_Word_Length word_length, UART_Stop_Bits stop_bits, UART_Parity parity) {
    uart_handle.Init.WordLength = word_length;
    uart_handle.Init.StopBits = stop_bits;
    uart_handle.Init.Parity = parity;
}

void Uart::set_instance() {
    if ((gpio_port == GPIOB && rx_pin == GPIO_PIN_7) && (gpio_port == GPIOA && tx_pin == GPIO_PIN_9)) {
        instance = USART1;
    }
    if ((gpio_port == GPIOA && rx_pin == GPIO_PIN_3) && (gpio_port == GPIOA && tx_pin == GPIO_PIN_2)) {
        instance = USART2;
    }
    if ((gpio_port == GPIOC && rx_pin == GPIO_PIN_7) && (gpio_port == GPIOC && tx_pin == GPIO_PIN_6)) {
        instance = USART6;
    }
}

void Uart::set_control_flow(UART_Ctl_Flow hw_ctl_flow) {
    uart_handle.Init.HwFlowCtl = hw_ctl_flow;
}

void Uart::set_sampling(UART_Over_Sampling over_sampling) {
    uart_handle.Init.OverSampling = over_sampling;
}

bool Uart::write(const char *tx_data) {
    std::string s(tx_data);
    s.append("\r\n");
    if (HAL_UART_Transmit(&uart_handle, (uint8_t *) s.c_str(), s.size(), uart_time_out) == HAL_OK) {
        return true;
    } else {
        return false;
    }

}

char *Uart::read() {
    memset(rx_buffer, 0, sizeof(rx_buffer));
    HAL_UART_Receive(&uart_handle, (uint8_t *)rx_buffer, sizeof(rx_buffer), 1000);
    return rx_buffer;
}




