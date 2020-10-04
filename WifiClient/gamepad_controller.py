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

        self.throttle_step = THROTTLE_STEP
        self.pitch_step = PITCH_STEP
        self.roll_step = ROLL_STEP
        self.yaw_step = YAW_STEP

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
            commands["desired_throttle"] -= self.throttle_step
            if commands["desired_throttle"] <= MIN_THROTTLE:
                commands["desired_throttle"] = MIN_THROTTLE
        if self.upper_triggers[1]:
            commands["desired_throttle"] += self.throttle_step
            if commands["desired_throttle"] >= MAX_THROTTLE:
                commands["desired_throttle"] = MAX_THROTTLE
        commands["desired_pitch"] += round(self.joysticks_axis[0]) * self.pitch_step
        if commands["desired_pitch"] <= MIN_PITCH:
            commands["desired_pitch"] = MIN_PITCH
        if commands["desired_pitch"] >= MAX_PITCH:
            commands["desired_pitch"] = MAX_PITCH

        commands["desired_roll"] += round(self.joysticks_axis[1]) * self.roll_step
        if commands["desired_roll"] <= MIN_ROLL:
            commands["desired_roll"] = MIN_ROLL
        if commands["desired_roll"] >= MAX_ROLL:
            commands["desired_roll"] = MAX_ROLL

        commands["desired_yaw"] += round(self.joysticks_axis[3]) * self.yaw_step
        if commands["desired_yaw"] <= MIN_YAW:
            commands["desired_yaw"] = MIN_YAW
        if commands["desired_yaw"] >= MAX_YAW:
            commands["desired_yaw"] = MAX_YAW
        return self.emergency_button