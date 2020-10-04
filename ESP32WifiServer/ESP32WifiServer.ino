#include "esp_camera.h"
#include <WiFi.h>

// Select camera model
//#define CAMERA_MODEL_WROVER_KIT
//#define CAMERA_MODEL_ESP_EYE
//#define CAMERA_MODEL_M5STACK_PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE
#define CAMERA_MODEL_AI_THINKER

#include "camera_pins.h"

const char* ssid = "ESP32_CAM";
const char* password = "freineda";

camera_config_t cam_config;

char buff[30];

String client_received_data = "";
char controller_received_data[30];
int data_index = 0;

WiFiServer server(82);
WiFiClient client;

void startCameraServer();

void setup() {
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  Serial.println("IP Address : ");
  Serial.println(WiFi.softAPIP());
  
  configure_esp_cam();
  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.softAPIP());
  Serial.println("' to connect");

  server.begin();
}

void loop() {
  client = server.available();
  if (client) {
    while(client.connected()) {
      transmit_client_data();
      transmit_controller_data(); 
    }
    client.stop();
  }
}

void transmit_client_data() {
  if (client.available() > 0) {
    char c = client.read();
    if (c != '\n') {
      client_received_data += c;
    } else {
      Serial.println(client_received_data);
      client_received_data = "";
    }
  }
}

void transmit_controller_data() {
  if (Serial.available() > 0) {
    char c = Serial.read();
    if (c != '\n' && c != '\r') {
      controller_received_data[data_index] = c;
      data_index++;
    } else {
      client.write(controller_received_data, sizeof(controller_received_data));
      data_index = 0;
    }
  }
}

void configure_esp_cam() {
  cam_config.ledc_channel = LEDC_CHANNEL_0;
  cam_config.ledc_timer = LEDC_TIMER_0;
  cam_config.pin_d0 = Y2_GPIO_NUM;
  cam_config.pin_d1 = Y3_GPIO_NUM;
  cam_config.pin_d2 = Y4_GPIO_NUM;
  cam_config.pin_d3 = Y5_GPIO_NUM;
  cam_config.pin_d4 = Y6_GPIO_NUM;
  cam_config.pin_d5 = Y7_GPIO_NUM;
  cam_config.pin_d6 = Y8_GPIO_NUM;
  cam_config.pin_d7 = Y9_GPIO_NUM;
  cam_config.pin_xclk = XCLK_GPIO_NUM;
  cam_config.pin_pclk = PCLK_GPIO_NUM;
  cam_config.pin_vsync = VSYNC_GPIO_NUM;
  cam_config.pin_href = HREF_GPIO_NUM;
  cam_config.pin_sscb_sda = SIOD_GPIO_NUM;
  cam_config.pin_sscb_scl = SIOC_GPIO_NUM;
  cam_config.pin_pwdn = PWDN_GPIO_NUM;
  cam_config.pin_reset = RESET_GPIO_NUM;
  cam_config.xclk_freq_hz = 20000000;
  cam_config.pixel_format = PIXFORMAT_JPEG;
  //init with high specs to pre-allocate larger buffers
  if(psramFound()){
    cam_config.frame_size = FRAMESIZE_UXGA;
    cam_config.jpeg_quality = 10;
    cam_config.fb_count = 2;
  } else {
    cam_config.frame_size = FRAMESIZE_SVGA;
    cam_config.jpeg_quality = 12;
    cam_config.fb_count = 1;
  }
#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif
  // camera init
  esp_err_t err = esp_camera_init(&cam_config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  //initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);//flip it back
    s->set_brightness(s, 1);//up the blightness just a bit
    s->set_saturation(s, -2);//lower the saturation
  }
  //drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_QVGA);

#if defined(CAMERA_MODEL_M5STACK_WIDE)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif
}
