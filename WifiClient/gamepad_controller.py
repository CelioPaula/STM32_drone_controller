import pygame, sys, time

joystick = None

upper_triggers = [0, 0]
joysticks_axis = [0, 0, 0, 0, 0, 0]

def init_joysticks():
    try:
        global joystick
        joystick = pygame.joystick.Joystick(0)
        print(joystick)
    except pygame.error as e:
        return False
    else:
        joystick.init()
        return True

def init_gamepad():
    pygame.init()
    pygame.joystick.init()
    if not init_joysticks():
        print("There is no connected gamepad. Please connect a gamepad and then restart the application")
        return False
    else :
        print("A gamepad is connected")
        return True

def deinit_gamepad():
    pygame.joystick.quit()
    pygame.quit()

def get_buttons_values():
    for event in pygame.event.get():
        get_joysticks_axis()
        get_upper_triggers()

def get_joysticks_axis():
    axis_values = []
    for axis_index in range(0, joystick.get_numaxes()):
        joysticks_axis[axis_index] = joystick.get_axis(axis_index)

def get_upper_triggers():
    upper_triggers[0] = joystick.get_button(4)
    upper_triggers[1] = joystick.get_button(5)

"""joystick = pygame.joystick.Joystick(0)
joystick.init()  # Initializes Joystick

# get count of joysticks=1, axes=27, buttons=19 for DualShock 3

joystick_count = pygame.joystick.get_count()
print("joystick_count")
print(joystick_count)
print("--------------")

numaxes = joystick.get_numaxes()
print("numaxes")
print(numaxes)
print("--------------")

numbuttons = joystick.get_numbuttons()
print("numbuttons")
print(numbuttons)
print("--------------")

loopQuit = False
while loopQuit == False:

    # test joystick axes and prints values
    for i in range(0, numaxes):
        print(str(joystick.get_axis(i)))

     # test controller buttons
    outstr = ""
    for i in range(0, numbuttons):
        button = joystick.get_button(i)
        outstr = outstr + str(i) + ":" + str(button) + "|"
    print(outstr)

    for event in pygame.event.get():
        if event.type == QUIT:
            loopQuit = True
        elif event.type == pygame.KEYDOWN:
            if event.key == pygame.K_ESCAPE:
                loopQuit = True

        # Returns Joystick Button Motion
        if event.type == pygame.JOYBUTTONDOWN:
            print("joy button down")
        if event.type == pygame.JOYBUTTONUP:
            print("joy button up")
        if event.type == pygame.JOYBALLMOTION:
            print("joy ball motion")
        # axis motion is movement of controller
        # dominates events when used
        #if event.type == pygame.JOYAXISMOTION:
    # print("joy axis motion")

    time.sleep(0.01)"""
"""pygame.quit()
sys.exit()"""