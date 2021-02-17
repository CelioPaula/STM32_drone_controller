#pragma once
#include "stm32f4xx_hal.h"
#include "clock.hpp"
#include <string>

// UART Instance (Those instances are based only on the STM32F401RE board)
typedef enum {
    // USART Instance 1 (RX : PA10 & TX : PA9)
    USART_Inst_1 = USART1_BASE,
    // USART Instance 2 (RX : PA3 & TX : PA2)
    USART_Inst_2 = USART2_BASE,
    // USART Instance 6 (RX : PC7 & TX : PC6)
    USART_Inst_6 = USART6_BASE,
}UART_Instance;

// UART frame word length
typedef enum {
    UART_8B_Word_Length = UART_WORDLENGTH_8B,
    UART_9B_Word_Length = UART_WORDLENGTH_9B,
}UART_Word_Length;

// UART frame parity bits
typedef enum {
    UART_Parity_None = UART_PARITY_NONE,
    UART_Parity_Even = UART_PARITY_EVEN,
    UART_Parity_Odd = UART_PARITY_ODD,
}UART_Parity;

// UART frame stop bits
typedef enum {
    UART_1_Stop_Bits = UART_STOPBITS_1,
    UART_2_Stop_Bits = UART_STOPBITS_2,
}UART_Stop_Bits;

// UART flow control
typedef enum {
    UART_NONE_Ctl = UART_HWCONTROL_NONE,
    UART_RTS_Ctl = UART_HWCONTROL_RTS,
    UART_CTS_Ctl = UART_HWCONTROL_CTS,
    UART_RTS_CTS_Ctl = UART_HWCONTROL_RTS_CTS,
}UART_Ctl_Flow;

// UART sampling method
typedef enum {
    UART_8_Oversampling = UART_OVERSAMPLING_8,
    UART_16_Oversampling = UART_OVERSAMPLING_16,
}UART_Over_Sampling;

class Uart {

    public:

        /*
         * @brief : UART class constructor
         * baudrate :  desired baudrate (9600, 115200...)
         * word_length : desired data word length
         * parity : desired parity bits
         * stop_bits : desired stop bits
         * ctl_flow : desired flow control
         * @return : GPIO output object
         */
        Uart(UART_Instance uart_instance, uint32_t baudrate, UART_Word_Length word_length, UART_Parity parity, UART_Stop_Bits stop_bits, UART_Ctl_Flow ctl_flow);

        /*
         * @brief : UART Initializer
         */
        void init();

        /*
         * @brief : This method initializes UART receives interruptions
         * preempt_priority : interruption preempt priority
         * sub_priority : interruption sub priority
         */
        void init_receives_interrupts(uint32_t preempt_priority, uint32_t sub_priority);

        /*
         * @brief : This method set the UART data frame with the desired parity and stop bits
         * word_length : total data word length
         * parity : desired parity bits
         * stop_bits : desired stop bits
         */
        void set_data_frame(UART_Word_Length word_length, UART_Parity parity, UART_Stop_Bits stop_bits);

        /*
         * @brief : This method set the UART flow control
         * ctl_flow : desired flow control
         */
        void set_ctl_flow(UART_Ctl_Flow ctl_flow);

        /*
         * @brief : This method set the UART sampling method
         * over_sampling : desired over sampling method
         */
        void set_over_sampling(UART_Over_Sampling over_sampling);

        /*
         * @brief : This method set the desired UART baudrate
         * baudrate : desired baudrate
         */
        void set_baudrate(uint32_t baudrate);

        /*
         * @brief : This function allows to write tx_data_size bytes from the tx_data bytes array on the UART tx pin
         * tx_data : bytes array to send
         * tx_data_size : number of bytes to send
         * @return : true if the entire message has been sent successfully
         */
        bool write(uint8_t *tx_data, uint32_t tx_data_size);

        /*
         * @brief : This function allows to write a tx_data string. This function is mainly used for debugging
         * tx_data : string to send
         */
        bool print(const char *tx_data);

        /*
         * @brief : This function allows to receive data on the rx uart pin. The data is stored inside the rx_buffer.
         * rx_buffer : buffer where the received data is stored
         * rx_buffer_size : size of the rx_buffer
         */
        bool read(uint8_t *rx_buffer, uint32_t rx_buffer_size, uint32_t max_receive_timeout);

        // UART handler typedef
        UART_HandleTypeDef uart_handler;
        // UART instance
        UART_Instance instance;

        // Max UART timeout
        const uint32_t uart_time_out = HAL_MAX_DELAY;

    private:

        /*
         * @brief : UART GPIO port and pins initializer
         * is_tx : true to initialize UART tx port and pins. False to initialize UART rx port and pins
         */
        void init_pinout(bool is_tx);

        /*
         * @brief : This method allows UART rx and tx pinout (port and pins) setting
         */
        void set_pinout();

        // UART RX GPIO port
        GPIO_TypeDef* rx_gpio_port;
        // UART RX GPIO pin
        uint16_t rx_pin;
        // UART TX GPIO port
        GPIO_TypeDef* tx_gpio_port;
        // UART TX GPIO pin
        uint16_t tx_pin;
};
// Used for debugging
extern Uart uart_debug;
