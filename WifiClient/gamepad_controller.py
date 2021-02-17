import pygame, sys, time
from drone_controller import *

THROTTLE_STEP = 2
PITCH_STEP = 2
ROLL_STEP = 2
YAW_STEP = 2

class Gamepad_controller:
    def __init__(self):
        self.upper_triggers = [0, 0]
        self.joysticks_axis = [0, 0, 0, 0, 0, 0]
        self.emergency_button = False
        self.joystick = None

        pygame.init()
        pygame.joystick.init()
        self.is_joystick_detected = self.init_joysticks()

    def is_connected(self):
        if not self.is_joystick_detected:
            return False
        else:
            return True

    def init_joysticks(self):
        try:
            self.joystick = pygame.joystick.Joystick(0)
        except pygame.error as e:
            return False
        else:
            self.joystick.init()
            return True

    def deinit(self):
        pygame.joystick.quit()
        pygame.quit()

    def get_buttons_values(self):
        for event in pygame.event.get():
            for axis_index in range(0, self.joystick.get_numaxes()):
                self.joysticks_axis[axis_index] = self.joystick.get_axis(axis_index)
            self.upper_triggers[0] = self.joystick.get_button(4)
            self.upper_triggers[1] = self.joystick.get_button(5)
            if self.joystick.get_button(10):
                self.emergency_button = True

    def set_controller_commands(self):
        self.get_buttons_values()
        if self.upper_triggers[0]:
            interface.commands["throttle"] -= THROTTLE_STEP
            if interface.commands["throttle"] <= MIN_THROTTLE:
                interface.commands["throttle"] = MIN_THROTTLE
        if self.upper_triggers[1]:
            interface.commands["throttle"] += THROTTLE_STEP
            if interface.commands["throttle"] >= MAX_THROTTLE:
                interface.commands["throttle"] = MAX_THROTTLE
        interface.commands["pitch"] += round(self.joysticks_axis[0]) * PITCH_STEP
        if interface.commands["pitch"] <= MIN_PITCH:
            interface.commands["pitch"] = MIN_PITCH
        if interface.commands["pitch"] >= MAX_PITCH:
            interface.commands["pitch"] = MAX_PITCH

        interface.commands["roll"] += round(self.joysticks_axis[1]) * ROLL_STEP
        if interface.commands["roll"] <= MIN_ROLL:
            interface.commands["roll"] = MIN_ROLL
        if interface.commands["roll"] >= MAX_ROLL:
            interface.commands["roll"] = MAX_ROLL

        interface.commands["yaw"] += round(self.joysticks_axis[3]) * YAW_STEP
        if interface.commands["yaw"] <= MIN_YAW:
            interface.commands["yaw"] = MIN_YAW
        if interface.commands["yaw"] >= MAX_YAW:
            interface.commands["yaw"] = MAX_YAW
        return self.emergency_button