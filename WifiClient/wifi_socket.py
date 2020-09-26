import socket
import time, errno
from enum import Enum

class MessageHeader(Enum):
	Configuration = 0x10
	Data = 0x11

class ConfigurationFlag(Enum):
	ControllerConfigured = 0x20

socket_receive_timeout = 2.0
sock = socket.socket()

def socket_connect(server_address, server_port):
    try:
        sock.connect((server_address, server_port))
    except OSError:
        print("Failed to connect to the Wifi Server")
        return False
    else:
        print("Successfully connected to the Wifi Server")
        sock.settimeout(socket_receive_timeout)
        return True

def socket_disconnect():
    print("Socket closed")
    sock.close()

def send_commands(data):
    # Set message header
    message = "{" + chr(MessageHeader.Data.value)
    # Set commands message
    message += ":" + str(data["desired_roll"]) + "|" + str(data["desired_pitch"]) + "|" + str(data["desired_yaw"]) + "|" + str(data["desired_throttle"]) + "}" + "\n"
    print(message + "\r")
    byte_message = bytes(message, 'utf-8')
    sock.send(byte_message)

def send_configuration(configuration):
    # Set message header
    message = "{" + chr(MessageHeader.Configuration.value)
    # Pitch parameters
    message += ":" + str(configuration["pitch_kp"]) + "|" + str(configuration["pitch_ki"]) + "|" + str(configuration["pitch_kd"]) + "|"
    # Roll parameters
    message += str(configuration["roll_kp"]) + "|" + str(configuration["roll_ki"]) + "|" + str(configuration["roll_kd"]) + "}\n"
    byte_message = bytes(message, 'utf-8')
    sock.send(byte_message)

def is_controller_configured():
    try:
        data = sock.recv(6)
    except socket.timeout as e:
        err = e.args[0]
        if err == "timed out":
            print("Receive configuration timeout\r")
        else:
            print(e)
            return False
    else:
        try:
            str_data = data.decode("utf-8")
        except:
            print("Decoding error")
        else:
            expected_result = "{" + chr(MessageHeader.Configuration.value) + ":" + chr(ConfigurationFlag.ControllerConfigured.value) + "}"
            if (str_data.find(expected_result) != -1):
                return True
    return False