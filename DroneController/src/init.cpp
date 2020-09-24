#include "init.hpp"

Brushless motor_FR = Brushless(GPIOB, GPIO_PIN_9, 2000, 200);
Brushless motor_BR = Brushless(GPIOB, GPIO_PIN_6, 2000, 200);
Brushless motor_FL = Brushless(GPIOA, GPIO_PIN_0, 2000, 200);
Brushless motor_BL = Brushless(GPIOA, GPIO_PIN_1, 2000, 200);
//Adc adc_input = Adc(GPIOA, GPIO_PIN_0, ADC_Resolution_12B, false);
// Init scheduler after brushless motors
Timer IMU_scheduler = Timer(1000, 8400);
IMU mpu6050 = IMU(GPIOC, GPIO_PIN_9, GPIOA, GPIO_PIN_8, GyroSpeed_250, AccelSpeed_4G);
Uart uart_com = Uart(GPIOA, GPIO_PIN_10, GPIO_PIN_9, 0, 0);
Uart uart_debug = Uart(GPIOA, GPIO_PIN_3, GPIO_PIN_2);
PID pid_roll = PID(4, 0, 0);
PID pid_pitch = PID(4, 0, 0);

void init_all() {

    HAL_Init();

    SystemClock_Config();

    uart_debug.default_init(115200);
    uart_com.default_init(115200);

    //Gpio_output led = Gpio_output(GPIOA, GPIO_PIN_5, Push_Pull, SpeedUpTo_2MHz);
    //Gpio_input btn = Gpio_input(GPIOC, GPIO_PIN_4, EXTIT_RisingEdge, Pull_Up);

    //led.init();
    //btn.init();
    //btn.set_interrupt_priority(0, 0);

    //adc_input.init();
    //adc_input.init(DMA_Priority_Low, 0, 0);
    //adc_input.start_adc();

    // Init the timers array before setting PWM and interrupt timers
    init_used_timers_array();

    motor_FR.init();
    motor_BR.init();
    motor_FL.init();
    motor_BL.init();

    if (mpu6050.init()) {
        mpu6050.configure();
        mpu6050.calibrate_gyro(1);
    }

    // Init scheduler after brushless motors and IMU
    IMU_scheduler.init();

    /* /!\ Start all interruptions after everything /!\ */

    IMU_scheduler.start_timer_interruption(1, 1);
    uart_com.start_reading_interrupt();

    //pid_roll.constrain(-400, 400);
    //pid_pitch.constrain(-400, 400);

}
