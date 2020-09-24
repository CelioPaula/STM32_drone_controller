import socket
import time, errno
from enum import Enum

class MessageHeader(Enum):
	Configuration = 0x10
	Data = 0x11

class ConfigurationFlag(Enum):
	ControllerConfigured = 0x20

socket_connection_timeout = 3.0
socket_receive_timeout = 0.1
sock = socket.socket()

def socket_connect(server_address, server_port):
    try:
        sock.settimeout(socket_connection_timeout)
        sock.connect((server_address, server_port))
    except socket.timeout:
        print("Failed to connect to the Wifi Server")
        return False
    else:
        print("Successfully connected to the Wifi Server")
        sock.settimeout(socket_receive_timeout)
        return True

def socket_disconnect():
    sock.close()

def send_commands(data):
    # Set message header
    message = "{" + chr(MessageHeader.Data.value)
    # Set commands message
    message += ":" + str(data["desired_roll"]) + "|" + str(data["desired_pitch"]) + "|" + str(data["desired_yaw"]) + "|" + str(data["desired_throttle"]) + "}" + "\n"
    print(message + "\r")
    sock.send(message + "\n")

def send_configuration(configuration):
    # Set message header
    message = "{" + chr(MessageHeader.Configuration.value)
    # Pitch parameters
    message += ":" + str(configuration["pitch_kp"]) + "|" + str(configuration["pitch_ki"]) + "|" + str(configuration["pitch_kd"]) + "|"
    # Roll parameters
    message += str(configuration["roll_kp"]) + "|" + str(configuration["roll_ki"]) + "|" + str(configuration["roll_kd"]) + "}"
    #print(message + "\r")
    sock.send(message + "\n")

def configure_controller(configuration):
    data = ""
    while True:
        send_configuration(configuration)
        try:
            data = sock.recv(10)
        except socket.timeout as e:
            err = e.args[0]
            if err == "timed out":
                print("Receive timeout\r")
                continue
            else:
                print(e)
                return False
        else:
            if (ord(data[0]) == ConfigurationFlag.ControllerConfigured.value):
                return True
    return False