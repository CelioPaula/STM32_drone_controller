#include "init.h"

void init_all() {

    HAL_Init();

    SystemClock_Config();

    uart.default_init(38400);
}
