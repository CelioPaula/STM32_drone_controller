from wifi_socket import *
from gamepad_controller import *
from drone_controller import *
import time

ESP_32_SERVER_ADDRESS = "192.168.4.1"
ESP_32_SERVER_PORT = 82


if __name__ == "__main__":
    if init_gamepad():
        #if(socket_connect(ESP_32_SERVER_ADDRESS, ESP_32_SERVER_PORT)):
        while True:
            get_buttons_values()
            set_controller_commands(joysticks_axis, upper_triggers)
            time.sleep(0.01)
            """if joysticks_axis and upper_triggers:
                print(upper_triggers)"""
            """if len(joysticks_values) != 0:
                print(joysticks_values)"""
        """if configure_controller():
            while True:
                joysticks_values = get_joysticks_axis()
                print(joysticks_values)"""



    socket_disconnect()
    deinit_gamepad()