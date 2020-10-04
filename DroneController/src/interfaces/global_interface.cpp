#include "global_interface.hpp"
#include "Drivers/uart.hpp"
#include "string.h"
#include "maths/math.hpp"

#define MAX_ROLL 2000
#define MAX_PITCH 2000
#define MAX_YAW 2000

DroneCommands commands = {MAX_ROLL/2, MAX_PITCH/2, MAX_YAW/2, 0};
ControllerConfiguration configuration = {0};
MessageHeader current_message_type;
bool send_new_message = false;
FlagMessage received_flag;


float battery_voltage = 0;

void process_receives() {
    std::string s(uart_com.rx_buffer);
    if (s.find("}") != std::string::npos && s.find("{") != std::string::npos) {
        std::string str_value = "";
        uint8_t command_offset = 0;
        float value = 0;
        bool start = false;
        for (char const &c : s) {
            if (start) {
                if (c != '|' && c != '{' && c != '}' && c != '\r' && c != '\n' && c != ':') {
                    if (c != Configuration && c != Data && c != Flag) {
                        str_value += c;
                    } else {
                        if (c == Configuration) {
                            current_message_type = Configuration;
                        }
                        if (c == Data) {
                            current_message_type = Data;
                        }
                        if (c == Flag) {
                            current_message_type = Flag;
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
                    if (c == '}') {
                        if (current_message_type == Configuration && command_offset >= sizeof(ControllerConfiguration)) {
                            send_new_message = true;
                            return;
                        }
                        if (current_message_type == Flag && (FlagMessage)str_value[0] == GetCurrentState) {
                            received_flag = (FlagMessage)str_value[0];
                            send_new_message = true;
                            return;
                        }
                    }
                    str_value = "";
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
        std::string feedback = "{";
        if (current_message_type == Configuration) {
            feedback.append(1, (char)Flag);
            feedback.append(1, ':');
            feedback.append(1, (char)ControllerConfigured);
            feedback.append(1, '}');
        }
        if (current_message_type == Flag && received_flag == GetCurrentState) {
            feedback.append(1, (char)Data);
            feedback.append(1, ':');
            feedback.append(std::to_string(commands.desired_roll));
            feedback.append("|");
            feedback.append(std::to_string(commands.desired_pitch));
            feedback.append("|");
            feedback.append(std::to_string(commands.desired_yaw));
            feedback.append("|");
            feedback.append(std::to_string(commands.desired_throttle));
            feedback.append("|");

            char buffer[10];
            snprintf(buffer, sizeof(buffer), "%.2f", battery_voltage);

            feedback.append(std::string(buffer));
            feedback.append("}");
        }
        uart_com.write(feedback.c_str());
    }
    send_new_message = false;
}

/*void get_commands() {
    commands.desired_throttle = map(commands.desired_throttle, 0, 2000, 0, 1200);
    commands.desired_roll = mapf((float)commands.desired_roll, 0, 2000, -10, 10);
    commands.desired_pitch = mapf((float)commands.desired_pitch, 0, 2000, -10, 10);
}*/


