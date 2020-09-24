from wifi_socket import *

pitch_kp = 1
pitch_ki = 2
pitch_kd = 3

roll_kp = 4
roll_ki = 5
roll_kd = 6

MAX_THROTTLE = 2000
MIN_THROTTLE = 0
THROTTLE_STEP = 5

MAX_ROLL = 2000
MIN_ROLL = 0
ROLL_STEP = 5

MAX_PITCH = 2000
MIN_PITCH = 0
PITCH_STEP = 5

MAX_YAW = 2000
MIN_YAW = 0
YAW_STEP = 5

commands = {"desired_roll" : MAX_ROLL/2, "desired_pitch" : MAX_PITCH/2, "desired_yaw" : MAX_YAW/2, "desired_throttle" : MIN_THROTTLE}

def configure_controller():
    configuration = {"pitch_kp" : pitch_kp, "pitch_kp" : pitch_ki, "pitch_kp" : pitch_kd, "pitch_kp" : roll_kp, "pitch_kp" : roll_ki, "pitch_kp" : roll_kd}
    send_configuration(configuration)

def set_controller_commands(joysticks_values, triggers_values):
    if triggers_values[0]:
        commands["desired_throttle"] -= THROTTLE_STEP
        if commands["desired_throttle"] <= MIN_THROTTLE:
            commands["desired_throttle"] = MIN_THROTTLE
    if triggers_values[1]:
        commands["desired_throttle"] += THROTTLE_STEP
        if commands["desired_throttle"] >= MAX_THROTTLE:
            commands["desired_throttle"] = MAX_THROTTLE

    commands["desired_pitch"] += int(joysticks_values[0] * PITCH_STEP)
    if commands["desired_pitch"] <= MIN_PITCH:
        commands["desired_pitch"] = MIN_PITCH
    if commands["desired_pitch"] >= MAX_PITCH:
        commands["desired_pitch"] = MAX_PITCH

    commands["desired_roll"] += int(joysticks_values[1] * ROLL_STEP)
    if commands["desired_roll"] <= MIN_ROLL:
        commands["desired_roll"] = MIN_ROLL
    if commands["desired_roll"] >= MAX_ROLL:
        commands["desired_roll"] = MAX_ROLL

    commands["desired_yaw"] += int(joysticks_values[3] * YAW_STEP)
    if commands["desired_yaw"] <= MIN_YAW:
        commands["desired_yaw"] = MIN_YAW
    if commands["desired_yaw"] >= MAX_YAW:
        commands["desired_yaw"] = MAX_YAW

    print(commands)
