#include "stm32f3xx_hal.h"
#include "Drivers/clock.hpp"


int main (void) {

    HAL_Init();

    SystemClock_Config();

    while (1) {

    }

    // Return 0 to satisfy compiler
    return 0;
}
