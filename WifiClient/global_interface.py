MAX_THROTTLE = 1000.0
MIN_THROTTLE = 0.0

MAX_ROLL = 15.0
MIN_ROLL = -15.0

MAX_PITCH = 15.0
MIN_PITCH = -15.0

MAX_YAW = 15.0
MIN_YAW = -15.0

PITCH_KP = 4
PITCH_KI = 0.0
PITCH_KD = 2

ROLL_KP = 4
ROLL_KI = 0
ROLL_KD = 2

YAW_KP = 0.0
YAW_KI = 0.0
YAW_KD = 0.0

class Global_interface:
    def __init__(self):
        self.commands = {"pitch" : 0, "roll" : 0, "yaw" : 0, "throttle" : MIN_THROTTLE}
        self.current_state = {"pitch" : 0.0, "roll" : 0.0, "yaw" : 0.0, "throttle" : 0.0, "battery_voltage" : 0.0}

interface = Global_interface()
