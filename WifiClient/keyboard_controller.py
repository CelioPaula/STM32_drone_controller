import sys, termios, tty, time
import threading
from global_interface import *

THROTTLE_STEP = 10
PITCH_STEP = 10
ROLL_STEP = 10
YAW_STEP = 10

class Keyboard_controller (threading.Thread):
    def __init__(self, time_btwn_key):
        self.value = ""
        self.time_btwn_key = time_btwn_key
        threading.Thread.__init__(self)

    def run(self):
        while (True):
            key_code = ord(self.getch())
            if key_code == 65:
                self.value = "UpperArrow"
            if key_code == 66:
                self.value = "DownArrow"
            if key_code == 67:
                self.value = "LeftArrow"
            if key_code == 68:
                self.value = "RightArrow"
            if key_code == 122:
                self.value = "Z"
            if key_code == 115:
                self.value = "S"
            if key_code == 113:
                self.value = "Q"
            if key_code == 100:
                self.value = "D"
            if key_code == 3:
                self.value = "CTRL+C"
                return
            # Escape key
            if key_code == 112:
                self.value = "P"
                return
            time.sleep(self.time_btwn_key)

    def getch(self):
        fd = sys.stdin.fileno()
        old_settings = termios.tcgetattr(fd)
        try:
            tty.setraw(sys.stdin.fileno())
            ch = sys.stdin.read(1)
        finally:
            termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
        return ch

    def exit_all(self):
        if self.value == "P":
            return True
        else:
            return False

    def set_controller_commands(self):
        if self.value != "":
            if (self.value == "UpperArrow"):
                interface.commands["throttle"] += THROTTLE_STEP
                if (interface.commands["throttle"] >= MAX_THROTTLE):
                    interface.commands["desired_throttle"] = MAX_THROTTLE
            if (self.value == "DownArrow"):
                interface.commands["throttle"] -= THROTTLE_STEP
                if (interface.commands["throttle"] <= MIN_THROTTLE):
                    interface.commands["throttle"] = MIN_THROTTLE
            if (self.value == "Z"):
                interface.commands["roll"] += ROLL_STEP
                if (interface.commands["roll"] >= MAX_ROLL):
                    interface.commands["roll"] = MAX_ROLL
            if (self.value == "S"):
                interface.commands["roll"] -= ROLL_STEP
                if (interface.commands["roll"] <= MIN_ROLL):
                    interface.commands["roll"] = MIN_ROLL
            if (self.value == "Q"):
                interface.commands["pitch"] -= PITCH_STEP
                if (interface.commands["pitch"] <= MIN_PITCH):
                    interface.commands["pitch"] = MIN_PITCH
            if (self.value == "D"):
                interface.commands["pitch"] += PITCH_STEP
                if (interface.commands["pitch"] >= MAX_PITCH):
                    interface.commands["pitch"] = MAX_PITCH
            if (self.value == "LeftArrow"):
                interface.commands["yaw"] -= YAW_STEP
            if (self.value == "RightArrow"):
                interface.commands["yaw"] += YAW_STEP
            if (self.value == "CTRL+C"):
                return True
        self.value = ""
        return False