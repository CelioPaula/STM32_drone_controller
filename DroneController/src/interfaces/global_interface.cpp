#include "global_interface.hpp"
#include "Drivers/uart.hpp"
#include "string.h"
#include "maths/math.hpp"

DroneCommands commands = {0};
ControllerConfiguration configuration = {0};
MessageHeader current_message_type;
bool send_new_message = false;

void process_messages() {
    std::string s(uart_com.rx_buffer);
    if (s.find("}") != std::string::npos && s.find("{") != std::string::npos) {
        std::string str_value = "";
        uint8_t command_offset = 0;
        float value = 0;
        bool start = false;
        for (char const &c : s) {
            if (start) {
                if (c != '|' && c != '{' && c != '}' && c != '\r' && c != '\n' && c != ':') {
                    if (c != Configuration && c != Data) {
                        str_value += c;
                    } else {
                        if (c == Configuration) {
                            current_message_type = Configuration;
                        }
                        if (c == Data) {
                            current_message_type = Data;
                        }
                    }
                }
                if (c == '|' || c == '}') {
                    if (current_message_type == Data) {
                        uint32_t command_value = std::stoi(str_value);
                        memcpy((uint8_t *)(&commands) + command_offset, &command_value, sizeof(uint32_t));
                        command_offset += sizeof(uint32_t);
                    }
                    if (current_message_type == Configuration) {
                        value = std::stof(str_value);
                        memcpy((uint8_t *)(&configuration) + command_offset, &value, sizeof(float));
                        command_offset += sizeof(float);
                    }
                    str_value = "";
                }
                if (c == '}') {
                    if (current_message_type == Configuration && command_offset >= sizeof(ControllerConfiguration)) {
                        send_new_message = true;
                        return;
                    }
                }
            } else {
                if (c == '{') {
                    start = true;
                }
            }
        }
    }
}

void send_feedback() {
    if (send_new_message) {
        if (current_message_type == Configuration) {
            std::string config_feedback = "{";
            config_feedback.append(1, (char)Configuration);
            config_feedback.append(1, ':');
            config_feedback.append(1, (char)ControllerConfigured);
            config_feedback.append(1, '}');
            uart_com.write(config_feedback.c_str());
        }
    }
    send_new_message = false;
}

/*void get_commands() {
    commands.desired_throttle = map(commands.desired_throttle, 0, 2000, 0, 1200);
    commands.desired_roll = mapf((float)commands.desired_roll, 0, 2000, -10, 10);
    commands.desired_pitch = mapf((float)commands.desired_pitch, 0, 2000, -10, 10);
}*/


