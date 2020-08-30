#ifndef INIT_H
#define INIT_H

#pragma once

#include "stm32f4xx_hal.h"
#include "Drivers/clock.hpp"
#include "Drivers/gpio.hpp"
#include "Drivers/uart.hpp"
#include "Drivers/adc.hpp"
#include "Drivers/dma.hpp"
#include "maths/math.hpp"
#include "actuators/brushless.hpp"

void init_all();

#endif // INIT_H
