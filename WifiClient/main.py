from gamepad_controller import *
from keyboard_controller import *
from drone_controller import *
import webbrowser

drone_controller = Drone_controller()
gamepad = Gamepad_controller()
key = Keyboard_controller(0)


def init_all():
    if drone_controller.connect():
        if gamepad.is_connected():
            print("A gamepad is connected\r")
        else:
            print("There is no connected gamepad. Keyboard control is now set\r")
        key.start()
        #webbrowser.open('http://192.168.4.1')
        drone_controller.configure()
        print("Drone controller successfully initialized")
        print("Opening Drone camera streaming...")

if __name__ == "__main__":
        emergency_request = False
        init_all()
        while True:
            if key.exit_all():
                print("Exiting application...")
                break
            if gamepad.is_connected():
                emergency_request = gamepad.set_controller_commands()
            emergency_request = key.set_controller_commands()
            if emergency_request:
                drone_controller.send_emergency_request()
                break
            if drone_controller.is_drone_thresholds_error_received():
                break
            drone_controller.send_drone_commands()
            time.sleep(0.05)
            drone_controller.get_current_state()
        key.join()
        drone_controller.disconnect()
        gamepad.deinit()