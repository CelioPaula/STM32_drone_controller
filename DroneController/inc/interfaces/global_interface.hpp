#pragma once
#include <string>
#include "stm32f4xx_hal.h"

typedef struct {
  uint32_t desired_roll;
  uint32_t desired_pitch;
  uint32_t desired_yaw;
  uint32_t desired_throttle;
}DroneCommands;

typedef struct {
  float pitch_kp;
  float pitch_ki;
  float pitch_kd;

  float roll_kp;
  float roll_ki;
  float roll_kd;
}ControllerConfiguration;

typedef enum {
    Configuration = 0x10,
    Data = 0x11,
}MessageHeader;

typedef enum {
    ControllerConfigured = 0x20,
}ConfigurationFlag;

void process_messages();
void send_feedback();

extern std::string received_commands;
extern DroneCommands commands;
extern ControllerConfiguration configuration;
extern MessageHeader current_message_type;
extern bool send_new_message;
