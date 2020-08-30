DEVICE = STM32F401xD
FLASH  = 0x08000000
OPENOCD_BOARD = st_nucleo_f4

USE_ST_CMSIS = true
USE_ST_HAL = true

# Include the main makefile
include ./STM32-base/make/common.mk
