from wifi_socket import *

pitch_kp = 1
pitch_ki = 2
pitch_kd = 3

roll_kp = 4
roll_ki = 5
roll_kd = 6

MAX_THROTTLE = 2000
MIN_THROTTLE = 0

MAX_ROLL = 2000
MIN_ROLL = 0

MAX_PITCH = 2000
MIN_PITCH = 0

MAX_YAW = 2000
MIN_YAW = 0

commands = {"desired_roll" : MAX_ROLL/2, "desired_pitch" : MAX_PITCH/2, "desired_yaw" : MAX_YAW/2, "desired_throttle" : MIN_THROTTLE}
feedback = {"roll": 0, "pitch": 0, "yaw": 0, "throttle": 0, "battery_voltage": 0}

def configure_controller():
    print("Waiting for drone controller configuration : pitch [Kp : {0}, Ki : {1}, Kd: : {2}] | roll [Kp : {3}, Ki : {4}, Kd: : {5}]\r".format(pitch_kp, pitch_ki, pitch_kd, roll_kp, roll_ki, roll_kd))
    while not is_controller_configured():
        configuration = {"pitch_kp" : pitch_kp, "pitch_ki" : pitch_ki, "pitch_kd" : pitch_kd, "roll_kp" : roll_kp, "roll_ki" : roll_ki, "roll_kd" : roll_kd}
        send_configuration(configuration)
    print("Drone controller configured\r")
    time.sleep(2.0)

def get_current_state():
    while not get_controller_feedback():
        send_flag(ConfigurationFlag.GetCurrentState.value)
        continue
    commands["desired_roll"] = feedback["roll"]
    commands["desired_pitch"] = feedback["pitch"]
    commands["desired_yaw"] = feedback["yaw"]
    commands["desired_throttle"] = feedback["throttle"]

def get_controller_feedback():
    start = False
    sub_string = ""
    str_data = receive_feedback()
    if str_data != "":
        if str_data.find("{") != -1 and str_data.find("}") != -1 and str_data.find(chr(MessageHeader.Data.value)) != -1:
            for c in str_data:
                if c == ':':
                    start = True
                if c == '}':
                    feedback_values = sub_string.split('|')
                    index = 0
                    print(str(feedback_values))
                    for key in feedback:
                        feedback[key] = float(feedback_values[index])
                        index += 1
                    return True
                if start and c != ':' and c != '}':
                    sub_string += c
    return False