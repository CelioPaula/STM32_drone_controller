from gamepad_controller import *
from drone_controller import *
from keyboard_controller import *

ESP_32_SERVER_ADDRESS = "192.168.4.1"
ESP_32_SERVER_PORT = 82


if __name__ == "__main__":
    if socket_connect(ESP_32_SERVER_ADDRESS, ESP_32_SERVER_PORT):
        emergency_request = False
        gamepad = Gamepad_controller()
        if gamepad.is_connected():
            print("A gamepad is connected\r")
        else:
            print("There is no connected gamepad. Keyboard control is now set\r")
        key = Keyboard_controller(0)
        key.start()

        configure_controller()
        get_current_state()

        while True:
            if key.exit_all():
                print("Exiting application...")
                break
            if gamepad.is_connected():
                emergency_request = gamepad.set_controller_commands()
            emergency_request = key.set_controller_commands()
            if emergency_request:
                send_emergency_request()
                break
            send_commands(commands)
            get_controller_feedback()
            print("COMMANDS : " + str(commands) + "\r")
            print("FEEDBACKS : " + str(feedback) + "\r\n")
        key.join()

        socket_disconnect()
        gamepad.deinit()