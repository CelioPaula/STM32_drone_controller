#pragma once
#include "stm32f4xx_hal.h"
#include "Drivers/gpio.hpp"
#include "Drivers/clock.hpp"
#include <string>

typedef enum {
    UART_8B_Word_Length = UART_WORDLENGTH_8B,
    UART_9B_Word_Length = UART_WORDLENGTH_9B,
}UART_Word_Length;

typedef enum {
    UART_Parity_None = UART_PARITY_NONE,
    UART_Parity_Even = UART_PARITY_EVEN,
    UART_Parity_Odd = UART_PARITY_ODD,
}UART_Parity;

typedef enum {
    UART_1_Stop_Bits = UART_STOPBITS_1,
    UART_2_Stop_Bits = UART_STOPBITS_2,
}UART_Stop_Bits;

typedef enum {
    UART_RX_Mode = UART_MODE_RX,
    UART_TX_Mode = UART_MODE_TX,
    UART_RX_TX_Mode = UART_MODE_TX_RX,
}UART_Mode;

typedef enum {
    UART_NONE_Ctl = UART_HWCONTROL_NONE,
    UART_RTS_Ctl = UART_HWCONTROL_RTS,
    UART_CTS_Ctl = UART_HWCONTROL_CTS,
    UART_RTS_CTS_Ctl = UART_HWCONTROL_RTS_CTS,
}UART_Ctl_Flow;

typedef enum {
    UART_8_Oversampling = UART_OVERSAMPLING_8,
    UART_16_Oversampling = UART_OVERSAMPLING_16,
}UART_Over_Sampling;

const uint32_t uart_time_out = HAL_MAX_DELAY;

class Uart {

    public:

        // Make sure to have both rx and tx pins on the same gpio port
        Uart(GPIO_TypeDef *gpio_port, uint16_t rx_pin, uint16_t tx_pin);

        Uart(GPIO_TypeDef *gpio_port, uint16_t rx_pin, uint16_t tx_pin, uint32_t preempt_priority, uint32_t sub_priority);

        void init(uint32_t baudrate);

        void default_init(uint32_t baudrate);

        void set_data_frame(UART_Word_Length word_length, UART_Stop_Bits stop_bits, UART_Parity parity);

        void set_control_flow(UART_Ctl_Flow hw_ctl_flow);

        void set_sampling(UART_Over_Sampling over_sampling);

        bool write(const char *tx_data);

        bool read();

        bool start_reading_interrupt();

#define UART_RX_BUFFER_SIZE 30

        char rx_buffer[UART_RX_BUFFER_SIZE];

        UART_HandleTypeDef uart_handler;

        USART_TypeDef *instance;

    private:

        void set_interrupt();

        GPIO_TypeDef* gpio_port;

        uint16_t rx_pin;
        uint16_t tx_pin;

        uint32_t preempt_priority;
        uint32_t sub_priority;

        bool use_interrupt;

        void init_pinout(bool is_tx);
        void set_instance();
};

extern Uart uart_debug;
extern Uart uart_com;