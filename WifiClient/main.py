from wifi_socket import *
from gamepad_controller import *
from drone_controller import *
import time

ESP_32_SERVER_ADDRESS = "192.168.4.1"
ESP_32_SERVER_PORT = 82


if __name__ == "__main__":
    if init_gamepad():
        if(socket_connect(ESP_32_SERVER_ADDRESS, ESP_32_SERVER_PORT)):
            configure_controller()
            while True:
                get_buttons_values()
                set_controller_commands(joysticks_axis, upper_triggers)
                send_commands(commands)
                time.sleep(0.01)
    socket_disconnect()
    deinit_gamepad()