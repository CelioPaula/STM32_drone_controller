#include "interfaces/drone_interface.hpp"
#include "uart.hpp"

void print_commands() {
    std::string s = "Pitch : ";
    s.append(std::to_string(commands.pitch));
    s.append(" Roll : ");
    s.append(std::to_string(commands.roll));
    s.append(" Yaw : ");
    s.append(std::to_string(commands.yaw));
    s.append(" Throttle : ");
    s.append(std::to_string((uint32_t)commands.throttle));
    uart_debug.print(s.c_str());
}
