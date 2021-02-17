#include "init.hpp"

Brushless motor_FR = Brushless(GPIOB, GPIO_PIN_9, 2000, 200);
Brushless motor_BR = Brushless(GPIOB, GPIO_PIN_6, 2000, 430);
Brushless motor_FL = Brushless(GPIOA, GPIO_PIN_0, 2000, 200);
Brushless motor_BL = Brushless(GPIOA, GPIO_PIN_1, 2000, 200);
//Adc adc_input = Adc(GPIOA, GPIO_PIN_0, ADC_Resolution_12B, false);
IMU mpu6050 = IMU(GPIOC, GPIO_PIN_9, GPIOA, GPIO_PIN_8, GyroSpeed_250, AccelSpeed_4G);
Uart uart_debug = Uart(USART_Inst_2, 9600, UART_8B_Word_Length, UART_Parity_None, UART_1_Stop_Bits, UART_NONE_Ctl);
ESP32 esp32 = ESP32();
PID pid_roll = PID();
PID pid_pitch = PID();
PID pid_yaw = PID();
uint32_t time_ms = 0;

PID_Configuration pid_pitch_configuration = {0};
PID_Configuration pid_roll_configuration = {0};
PID_Configuration pid_yaw_configuration = {0};
Drone_State drone_state = {0};
Commands commands = {0};

void init_all() {

    HAL_Init();

    SystemClock_Config();

    //Gpio_output led = Gpio_output(GPIOA, GPIO_PIN_5, Push_Pull, SpeedUpTo_2MHz);
    //Gpio_input btn = Gpio_input(GPIOC, GPIO_PIN_4, EXTIT_RisingEdge, Pull_Up);

    //led.init();
    //btn.init();
    //btn.set_interrupt_priority(0, 0);

    //adc_input.init();
    //adc_input.init(DMA_Priority_Low, 0, 0);
    //adc_input.start_adc();


    uart_debug.init();
    esp32.init();

    // Init the timers array before setting PWM and interrupt timers
    init_used_timers_array();

    motor_FR.init();
    motor_BR.init();
    motor_FL.init();
    motor_BL.init();

    if (mpu6050.init()) {
        mpu6050.configure();
        mpu6050.calibrate_gyro();
        mpu6050.calibrate_accel();
    }


    /* /!\ Start all interruptions after everything /!\ */
    esp32.it_start_reading();

    //pid_roll.constrain(-400, 400);
    //pid_pitch.constrain(-400, 400);
}
