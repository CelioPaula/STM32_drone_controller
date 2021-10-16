import socket
from wifi import *
import time, errno
from enum import Enum
import struct
import crc8
from global_interface import *

SOCKET_RECEIVE_TIMEOUT = 0.05

ESP_32_SERVER_ADDRESS = "192.168.4.1"
ESP_32_SERVER_PORT = 82

ESP32_WIFI_NETWORK_NAME = "ESP32_CAM"

# bytes
MAX_COMMAND_PARAMETER_SIZE = 50
# MAX_COMMAND_BUFFER_SIZE = START_BYTE + OP_CODE + MAX_COMMAND_PARAMETER_SIZE + CHECKSUM
# MAX_COMMAND_BUFFER_SIZE = 1 + 1 + 64 + 1 = 64 + 3
MAX_COMMAND_BUFFER_SIZE = MAX_COMMAND_PARAMETER_SIZE + 3

START_BYTE = 0xAA

class OPCode(Enum):
    Command_ACK = 0x01
    DronePIDConfiguration = 0x10
    DroneCurrentState = 0x11
    DroneCommands = 0x12
    EmergencyRequest = 0x13
    DroneThresholdsReached = 0x14

class Drone_controller:
    def __init__(self):
        self.ip_address = ESP_32_SERVER_ADDRESS
        self.port = ESP_32_SERVER_PORT
        self.is_connected = False
        self.sock = socket.socket()
        self.received_op_code = 0x00
        self.received_parameter = []
        self.print_counter = 0

    def connect(self):
        if (connect_to_wifi_network(ESP32_WIFI_NETWORK_NAME)):
            try:
                self.sock.connect((ESP_32_SERVER_ADDRESS, ESP_32_SERVER_PORT))
            except:
                print("Failed to connect to the Wifi Server\r")
                return False
            else:
                print("Successfully connected to the Wifi Server\r")
                self.sock.settimeout(SOCKET_RECEIVE_TIMEOUT)
                self.is_connected = True
                return True

    def disconnect(self):
        if (self.is_connected):
            print("Socket closed")
            self.is_connected = False
            self.sock.close()

    def send_command(self, op_code, parameter, parameter_type = "Byte"):
        byte_array = []
        command_buffer = [0] * MAX_COMMAND_BUFFER_SIZE
        command_buffer[0] = START_BYTE
        command_buffer[1] = op_code
        if parameter_type == "Float":
            for list_elemnt in parameter:
                byte_array += bytes(struct.pack('<f', list_elemnt))
            command_buffer[2:2 + len(byte_array)] = byte_array
        if parameter_type == "Integer":
            for list_elemnt in parameter:
                byte_array += list_elemnt.to_bytes(4, 'little')
            command_buffer[2:2 + len(byte_array)] = byte_array
        if parameter_type == "Byte":
            command_buffer[2:2 + len(parameter)] = parameter
        crc8 = self.calculate_crc8(command_buffer[:MAX_COMMAND_BUFFER_SIZE - 1])
        command_buffer[MAX_COMMAND_BUFFER_SIZE - 1] = crc8
        try:
            self.sock.send(bytes(command_buffer))
        except socket.timeout as e:
            err = e.args[0]
            if err == "timed out":
                print("Send command timeout\r")
            else:
                print(e)

    def calculate_checksum(self, buffer):
        sum = 0
        for i in range(1, len(buffer) - 1):
           sum = sum + buffer[i]
        checksum = 1 + ~sum
        checksum = checksum & 0xFF
        return checksum

    def calculate_crc8(self, buffer):
        hash = crc8.crc8()
        hash.update(bytes(buffer))
        return int.from_bytes(hash.digest(), byteorder='little')

    def evaluate_checksum(self, buffer):
        checksum = self.calculate_checksum(buffer)
        if checksum == buffer[MAX_COMMAND_BUFFER_SIZE - 1]:
            return True
        else:
            return False

    def evaluate_crc8(self, buffer):
        crc8 = self.calculate_crc8(buffer[:MAX_COMMAND_BUFFER_SIZE - 1])
        if crc8 == buffer[MAX_COMMAND_BUFFER_SIZE - 1]:
            return True
        else:
            return False

    def receive_feedback(self):
        try:
            data = self.sock.recv(MAX_COMMAND_BUFFER_SIZE)
            start_reading = False
            frame_buffer = []
            frame_buffer_index = 0
            for i in range(0, MAX_COMMAND_BUFFER_SIZE):
                if data[i] == START_BYTE:
                    start_reading = True
                if start_reading:
                    frame_buffer.append(data[i])
                    frame_buffer_index += 1
            if start_reading:
                for i in range(0, MAX_COMMAND_BUFFER_SIZE - frame_buffer_index):
                    frame_buffer.append(data[i])
                if self.evaluate_crc8(frame_buffer):
                    self.received_op_code = frame_buffer[1]
                    self.received_parameter = frame_buffer[2:MAX_COMMAND_PARAMETER_SIZE + 2]
                    return True
        except socket.timeout as e:
            err = e.args[0]
            """if err == "timed out":
                #print("Receive feedback timeout\r")
            else:
                print(e)"""
        return False

    def configure(self):
        print("Waiting for drone controller configuration : pitch [Kp : {0}, Ki : {1}, Kd: : {2}] | roll [Kp : {3}, Ki : {4}, Kd: : {5}] | yaw [Kp : {6}, Ki : {7}, Kd: : {8}]\r"
              .format(PITCH_KP, PITCH_KI, PITCH_KD, ROLL_KP, ROLL_KI, ROLL_KD, YAW_KP, YAW_KI, YAW_KD))
        configuration = [PITCH_KP, PITCH_KI, PITCH_KD, ROLL_KP, ROLL_KI, ROLL_KD, YAW_KP, YAW_KI, YAW_KD]
        target_parameter = [0] * MAX_COMMAND_PARAMETER_SIZE
        target_parameter[0] = OPCode.DronePIDConfiguration.value
        while OPCode.Command_ACK.value != self.received_op_code and target_parameter != self.received_parameter:
            self.send_command(OPCode.DronePIDConfiguration.value, configuration, parameter_type="Float")
            time.sleep(0.1)
            self.receive_feedback()
        while OPCode.DroneCurrentState.value != self.received_op_code:
            self.send_current_state_request()
            time.sleep(0.1)
            self.get_current_state()
            interface.commands["throttle"] = interface.current_state["throttle"]

    def send_drone_commands(self):
        commands = []
        if (interface.commands["throttle"] > 0) :
            commands = [interface.commands["pitch"], interface.commands["roll"], interface.commands["yaw"], interface.commands["throttle"]]
        self.send_command(OPCode.DroneCommands.value, commands, parameter_type="Float")
        #print("Drone commands :" + str(commands) + "\r\n")

    def get_current_state(self):
        if self.receive_feedback() and OPCode.DroneCurrentState.value == self.received_op_code:
            interface.current_state["pitch"] = struct.unpack('<f', bytes(self.received_parameter[0:4]))[0]
            interface.current_state["roll"] = struct.unpack('<f', bytes(self.received_parameter[4:8]))[0]
            interface.current_state["yaw"] = struct.unpack('<f', bytes(self.received_parameter[8:12]))[0]
            interface.current_state["throttle"] = struct.unpack('<f', bytes(self.received_parameter[12:16]))[0]
            interface.current_state["battery_voltage"] = struct.unpack('<f', bytes(self.received_parameter[16:20]))[0]
            round_values = [round(value, 1) for value in list(interface.current_state.values())]
            print(str(self.print_counter) + ": Drone current state :" + str(round_values) + "\r\n")
            self.print_counter += 1

    def send_current_state_request(self):
        self.send_command(OPCode.DroneCurrentState.value, [])

    def send_emergency_request(self):
        target_parameter = [0] * MAX_COMMAND_PARAMETER_SIZE
        target_parameter[0] = OPCode.EmergencyRequest.value
        print("Sending emergency request...")
        while OPCode.Command_ACK.value != self.received_op_code and target_parameter != self.received_parameter:
            self.send_command(OPCode.EmergencyRequest.value, [])
            self.receive_feedback()
        print("Emergency request received")

    def is_drone_thresholds_error_received(self):
        if self.receive_feedback() and OPCode.DroneThresholdsReached.value == self.received_op_code:
            print("Drone angles thresholds reached")
            return True
        else:
            return False