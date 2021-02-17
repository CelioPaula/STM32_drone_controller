MAX_THROTTLE = 1000.0
MIN_THROTTLE = 0.0

MAX_ROLL = 2.0
MIN_ROLL = 0.0

MAX_PITCH = 3.0
MIN_PITCH = 0.0

MAX_YAW = 4.0
MIN_YAW = 0.0

PITCH_KP = 1.0
PITCH_KI = 2.0
PITCH_KD = 3.0

ROLL_KP = 4.0
ROLL_KI = 5.0
ROLL_KD = 6.0

YAW_KP = 7.0
YAW_KI = 8.0
YAW_KD = 9.0

class Global_interface:
    def __init__(self):
        self.commands = {"pitch" : MAX_PITCH/2, "roll" : MAX_ROLL/2, "yaw" : MAX_YAW/2, "throttle" : MIN_THROTTLE}
        self.current_state = {"pitch" : 0.0, "roll" : 0.0, "yaw" : 0.0, "throttle" : 0.0, "battery_voltage" : 0.0}

interface = Global_interface()
