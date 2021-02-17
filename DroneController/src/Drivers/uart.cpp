#include "Drivers/uart.hpp"

Uart::Uart(UART_Instance uart_instance, uint32_t baudrate, UART_Word_Length word_length, UART_Parity parity, UART_Stop_Bits stop_bits, UART_Ctl_Flow ctl_flow) {
    instance = uart_instance;
    uart_handler.Instance = (USART_TypeDef *) uart_instance;
    uart_handler.Init.Mode = UART_MODE_TX_RX;
    set_pinout();
    set_data_frame(word_length, parity, stop_bits);
    set_ctl_flow(ctl_flow);
    set_over_sampling(UART_8_Oversampling);
    set_baudrate(baudrate);
}

void Uart::init() {
    gpio_clock_init(rx_gpio_port);
    gpio_clock_init(tx_gpio_port);
    uart_clock_init((USART_TypeDef *) instance);
    init_pinout(true);
    init_pinout(false);
    HAL_UART_Init(&uart_handler);
}

void Uart::set_pinout() {
    if (instance == USART_Inst_1) {
        rx_pin = GPIO_PIN_10;
        rx_gpio_port = GPIOA;
        tx_pin = GPIO_PIN_9;
        tx_gpio_port = GPIOA;
    }
    if (instance == USART_Inst_2) {
        rx_pin = GPIO_PIN_3;
        rx_gpio_port = GPIOA;
        tx_pin = GPIO_PIN_2;
        tx_gpio_port = GPIOA;
    }
    if (instance == USART_Inst_6) {
        rx_pin = GPIO_PIN_7;
        rx_gpio_port = GPIOC;
        tx_pin = GPIO_PIN_6;
        tx_gpio_port = GPIOC;
    }
}

void Uart::init_receives_interrupts(uint32_t preempt_priority, uint32_t sub_priority) {
    if (instance == USART_Inst_1) {
        HAL_NVIC_SetPriority(USART1_IRQn, preempt_priority, sub_priority);
        HAL_NVIC_EnableIRQ(USART1_IRQn);
    }
    if (instance == USART_Inst_2) {
        HAL_NVIC_SetPriority(USART2_IRQn, preempt_priority, sub_priority);
        HAL_NVIC_EnableIRQ(USART2_IRQn);
    }
    if (instance == USART_Inst_6) {
        HAL_NVIC_SetPriority(USART6_IRQn, preempt_priority, sub_priority);
        HAL_NVIC_EnableIRQ(USART6_IRQn);
    }
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
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    if (instance == USART_Inst_1) {
        GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    }
    if (instance == USART_Inst_2) {
        GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    }
    if (instance == USART_Inst_6) {
        GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
    }

    if (is_tx) {
        HAL_GPIO_Init(tx_gpio_port, &GPIO_InitStruct);
    } else {
        HAL_GPIO_Init(rx_gpio_port, &GPIO_InitStruct);
    }
}

void Uart::set_data_frame(UART_Word_Length word_length, UART_Parity parity, UART_Stop_Bits stop_bits) {
    uart_handler.Init.WordLength = word_length;
    uart_handler.Init.StopBits = stop_bits;
    uart_handler.Init.Parity = parity;
}

void Uart::set_ctl_flow(UART_Ctl_Flow ctl_flow) {
    uart_handler.Init.HwFlowCtl = ctl_flow;
}

void Uart::set_over_sampling(UART_Over_Sampling over_sampling) {
    uart_handler.Init.OverSampling = over_sampling;
}

void Uart::set_baudrate(uint32_t baudrate) {
    uart_handler.Init.BaudRate = baudrate;
}

bool Uart::write(uint8_t *tx_data, uint32_t tx_data_size) {
    if (HAL_UART_Transmit(&uart_handler, (uint8_t*) tx_data, tx_data_size, 10) == HAL_OK) {
        return true;
    } else {
        return false;
    }
}

bool Uart::print(const char *tx_data) {
    std::string s(tx_data);
    s.append("\r\n");
    if (HAL_UART_Transmit(&uart_handler, (uint8_t *) s.c_str(), s.size(), uart_time_out) == HAL_OK) {
        return true;
    } else {
        return false;
    }
}

bool Uart::read(uint8_t *rx_buffer, uint32_t rx_buffer_size, uint32_t max_receive_timeout) {
    if (HAL_UART_Receive(&uart_handler, (uint8_t *)rx_buffer, rx_buffer_size, max_receive_timeout) == HAL_OK) {
        return true;
    } else {
        return false;
    }
}
