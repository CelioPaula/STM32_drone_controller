#include "global_interface.hpp"
#include "Drivers/uart.hpp"
#include "string.h"
#include "maths/math.hpp"

DroneCommands commands = {0};
ControllerConfiguration configuration = {0};
MessageHeader current_message_type;

void process_messages() {
    std::string s(uart_com.rx_buffer);
    //uart_debug.write(s.c_str());
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
                if (c == '|') {
                    /*if (current_message_type == Data) {
                        uint32_t command_value = std::stoi(str_value);
                        memcpy(&commands + command_offset, &command_value, sizeof(uint32_t));
                        command_offset += sizeof(uint32_t);
                    }*/
                    if (current_message_type == Configuration) {
                        value = std::stof(str_value);
                        memcpy(&(configuration) + command_offset, &value, sizeof(float));
                        command_offset += sizeof(float);
                    }
                    str_value = "";
                }
                if (c == '}') {
                    str_value = "";
                    start = false;
                }
            } else {
                if (c == '{') {
                    start = true;
                }
            }
        }
    }
}

/*void get_commands() {
    commands.desired_throttle = map(commands.desired_throttle, 0, 2000, 0, 1200);
    commands.desired_roll = mapf((float)commands.desired_roll, 0, 2000, -10, 10);
    commands.desired_pitch = mapf((float)commands.desired_pitch, 0, 2000, -10, 10);
}*/


