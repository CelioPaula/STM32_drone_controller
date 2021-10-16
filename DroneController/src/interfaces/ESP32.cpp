#include "ESP32.hpp"
#include "interfaces/drone_interface.hpp"
#include <string.h>

ESP32::ESP32() :
    uart_com(Uart(UART_INST, BAUDRATE, WORD_LENGTH, PARITY, STOP_BITS, CTL_FLOW)) {
    receive_buffer_index = 0;
    command_payload = {0};
    emergency_request_received = false;
}

void ESP32::init() {
    uart_com.init();
    uart_com.init_receives_interrupts(PREEMPT_PRIORITY, SUB_PRIORITY);
}

bool ESP32::it_start_reading() {
    if (HAL_UART_Receive_IT(&uart_com.uart_handler, receive_buffer, MAX_COMMAND_BUFFER_SIZE) == HAL_OK) {
        return true;
    } else {
        return false;
    }
}

bool ESP32::send_command(uint8_t op_code, uint8_t *parameter) {
    // [0] : Start byte (0xAA default)
    // 1 byte size
    command_buffer[0] = START_BYTE;

    // [1] : OP Code (Command or command ID)
    // 1 byte size
    command_buffer[1] = op_code;

    // [2] : Parameter (Command or command data payload)
    // 64 bytes size
    memcpy(&command_buffer[2], parameter, MAX_COMMAND_PARAMETER_SIZE);

    // [32 + 2] : Checksum of packet
    // 1 byte size
    command_buffer[MAX_COMMAND_PARAMETER_SIZE + 2] = calculate_crc8(crc_initializer, command_buffer, MAX_COMMAND_BUFFER_SIZE - 1);

    HAL_UART_Transmit_IT(&uart_com.uart_handler, command_buffer, MAX_COMMAND_BUFFER_SIZE);
    /*if (uart_com.write(command_buffer, MAX_COMMAND_BUFFER_SIZE)) {
        return true;
    }*/
    return true;
}

bool ESP32::process_receive() {
    uint8_t frame_buffer[MAX_COMMAND_BUFFER_SIZE] = {0};
    uint32_t frame_buffer_index = 0;
    bool start_reading = false;
    for (uint8_t i = 0; i<MAX_COMMAND_BUFFER_SIZE; i++) {
        if (receive_buffer[i] == START_BYTE) {
            start_reading = true;
        }
        if (start_reading) {
            frame_buffer[frame_buffer_index] = receive_buffer[i];
            frame_buffer_index++;
        }
    }
    if (start_reading) {
        for (uint8_t i = 0; i<MAX_COMMAND_BUFFER_SIZE - frame_buffer_index; i++) {
            frame_buffer[frame_buffer_index + i] = receive_buffer[i];
        }
        /*std::string s;
        for (uint8_t i = 0; i<MAX_COMMAND_BUFFER_SIZE; i++) {
            s.append(std::to_string(frame_buffer[i]));
        }*/
        //uart_debug.print(s.c_str());
        if (evaluate_crc8(frame_buffer, MAX_COMMAND_BUFFER_SIZE)) {
            command_payload.op_code = frame_buffer[1];
            memcpy(command_payload.parameter, &frame_buffer[2], MAX_COMMAND_PARAMETER_SIZE);
            return true;
        }
    }
    return false;

}

uint8_t ESP32::calculate_checksum(uint8_t *buffer) {
    uint8_t sum = 0;
    for (uint32_t i = 1; i <= MAX_COMMAND_BUFFER_SIZE - 2; i++) {
        sum += buffer[i];
    }
    return 1 + ~(sum);
}

uint8_t ESP32::calculate_crc8(uint8_t crc, uint8_t *buffer, uint32_t buffer_size) {
    while(buffer_size--) {
        crc = crc ^ *buffer++; // Apply Byte
        crc = crc_table[crc & 0xFF]; // One round of 8-bits
    }
    return crc;
}

bool ESP32::evaluate_crc8(uint8_t *buffer, uint32_t buffer_size) {
    uint8_t crc8 = calculate_crc8(crc_initializer, buffer, buffer_size-1);
    if (buffer[buffer_size - 1] == crc8) {
        return true;
    } else {
        return false;
    }
}

bool ESP32::evaluate_checksum(uint8_t *buffer, uint32_t buffer_size) {
    uint8_t checksum = calculate_checksum(buffer);
    if (buffer[buffer_size - 1] == checksum) {
        return true;
    } else {
        return false;
    }
}

bool ESP32::is_configuration_received() {
    if (command_payload.op_code == DronePIDConfiguration) {
        return true;
    } else {
        return false;
    }
}

bool ESP32::get_PID_configuration() {
    if (command_payload.op_code == DronePIDConfiguration) {
        memcpy((uint8_t *) &pid_pitch_configuration, &command_payload.parameter[0], sizeof(PID_Configuration));
        memcpy((uint8_t *) &pid_roll_configuration, &command_payload.parameter[3 * sizeof(float)], sizeof(PID_Configuration));
        memcpy((uint8_t *) &pid_yaw_configuration, &command_payload.parameter[6 * sizeof(float)], sizeof(PID_Configuration));
        return true;
    }
    return false;
}

bool ESP32::is_state_request_received() {
    if (command_payload.op_code == DroneCurrentState) {
        return true;
    } else {
        return false;
    }
}

void ESP32::send_current_state() {
    uint8_t parameter[MAX_COMMAND_PARAMETER_SIZE] = {0};
    memcpy(parameter, (uint8_t *) &drone_state, sizeof(Drone_State));
    send_command(DroneCurrentState, parameter);
}

void ESP32::send_ack(uint32_t ack_op_code) {
    uint8_t parameter[MAX_COMMAND_PARAMETER_SIZE] = {0};
    parameter[0] = ack_op_code;
    send_command(Command_ACK, parameter);
}

bool ESP32::are_drone_commands_received() {
    if (command_payload.op_code == DroneCommands) {
        return true;
    } else {
        return false;
    }
}

void ESP32::get_drone_received_commands() {
    memcpy((uint8_t *)&commands, &command_payload.parameter, sizeof(Commands));
}

bool ESP32::is_emergency_request_received() {
    if (command_payload.op_code == EmergencyRequest) {
        emergency_request_received = true;
        return true;
    } else {
        return false;
    }
}

void ESP32::send_thresholds_reached_error() {
    uint8_t parameter[MAX_COMMAND_PARAMETER_SIZE] = {0};
    send_command(DroneThresholdsReached, parameter);
}

extern "C" {
    // Add the ESP32 associated UART instance IRQ Handler function like the following one :
    void USART1_IRQHandler(void) {
        if (esp32.uart_com.instance == USART_Inst_1) {
            HAL_UART_IRQHandler(&esp32.uart_com.uart_handler);
            HAL_UART_Receive_IT(&esp32.uart_com.uart_handler, esp32.receive_buffer, esp32.MAX_COMMAND_BUFFER_SIZE);
            HAL_UART_Transmit_IT(&esp32.uart_com.uart_handler, esp32.command_buffer, esp32.MAX_COMMAND_BUFFER_SIZE);
        }
    }
}
