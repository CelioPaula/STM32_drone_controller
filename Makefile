DEVICE = STM32F303xD
FLASH  = 0x08000000

USE_ST_CMSIS = true
USE_ST_HAL = true

# Include the main makefile
include ./STM32-base/make/common.mk
