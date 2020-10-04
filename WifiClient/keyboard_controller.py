import sys, termios, tty, time
import threading
from drone_controller import *

THROTTLE_STEP = 10
PITCH_STEP = 10
ROLL_STEP = 10
YAW_STEP = 10

class Keyboard_controller (threading.Thread):
    def __init__(self, time_btwn_key):
        self.value = ""
        self.time_btwn_key = time_btwn_key

        self.throttle_step = THROTTLE_STEP
        self.pitch_step = PITCH_STEP
        self.roll_step = ROLL_STEP
        self.yaw_step = YAW_STEP

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
                commands["desired_throttle"] += self.throttle_step
                if (commands["desired_throttle"] >= MAX_THROTTLE):
                    commands["desired_throttle"] = MAX_THROTTLE
            if (self.value == "DownArrow"):
                commands["desired_throttle"] -= self.throttle_step
                if (commands["desired_throttle"] <= MIN_THROTTLE):
                    commands["desired_throttle"] = MIN_THROTTLE
            if (self.value == "Z"):
                commands["desired_roll"] += self.roll_step
                if (commands["desired_roll"] >= MAX_ROLL):
                    commands["desired_roll"] = MAX_ROLL
            if (self.value == "S"):
                commands["desired_roll"] -= self.roll_step
                if (commands["desired_roll"] <= MIN_ROLL):
                    commands["desired_roll"] = MIN_ROLL
            if (self.value == "Q"):
                commands["desired_pitch"] -= self.pitch_step
                if (commands["desired_pitch"] <= MIN_PITCH):
                    commands["desired_pitch"] = MIN_PITCH
            if (self.value == "D"):
                commands["desired_pitch"] += self.pitch_step
                if (commands["desired_pitch"] >= MAX_PITCH):
                    commands["desired_pitch"] = MAX_PITCH
            if (self.value == "LeftArrow"):
                commands["desired_yaw"] -= self.yaw_step
            if (self.value == "RightArrow"):
                commands["desired_yaw"] += self.yaw_step
            if (self.value == "CTRL+C"):
                return True
        self.value = ""
        return False