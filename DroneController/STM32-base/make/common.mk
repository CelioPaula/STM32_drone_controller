# Check to make sure that the required variables are set
ifndef DEVICE
    $(error Please set the required DEVICE variable in your makefile.)
endif

ifndef FLASH
    $(error Please set the required FLASH variable in your makefile.)
endif

# Standard values for (linked) STM32-base folders
STM32_BASE_PATH   ?= ./STM32-base
STM32_CUBE_PATH   ?= ./STM32-base-STM32Cube

# STM32-base sub-folders
BASE_LINKER   = $(STM32_BASE_PATH)/linker
BASE_MAKE     = $(STM32_BASE_PATH)/make
BASE_STARTUP  = $(STM32_BASE_PATH)/startup

# Standard values for project folders
BIN_FOLDER ?= ./bin
OBJ_FOLDER ?= ./obj
SRC_FOLDER ?= ./src
INC_FOLDER ?= ./inc

# Determine the series folder name
include $(BASE_MAKE)/series-folder-name.mk

# Include the series-specific makefile
include $(BASE_MAKE)/$(SERIES_FOLDER)/common.mk
MAPPED_DEVICE ?= $(DEVICE)

# The toolchain path, defaults to using the globally installed toolchain
ifdef TOOLCHAIN_PATH
    TOOLCHAIN_SEPARATOR = /
endif

TOOLCHAIN_PATH      ?= ./tools/bin/
TOOLCHAIN_SEPARATOR ?=

CC      = $(TOOLCHAIN_PATH)$(TOOLCHAIN_SEPARATOR)arm-none-eabi-gcc
CXX     = $(TOOLCHAIN_PATH)$(TOOLCHAIN_SEPARATOR)arm-none-eabi-g++
LD      = $(TOOLCHAIN_PATH)$(TOOLCHAIN_SEPARATOR)arm-none-eabi-ld -v
AR      = $(TOOLCHAIN_PATH)$(TOOLCHAIN_SEPARATOR)arm-none-eabi-ar
AS      = $(TOOLCHAIN_PATH)$(TOOLCHAIN_SEPARATOR)arm-none-eabi-gcc
OBJCOPY = $(TOOLCHAIN_PATH)$(TOOLCHAIN_SEPARATOR)arm-none-eabi-objcopy
OBJDUMP = $(TOOLCHAIN_PATH)$(TOOLCHAIN_SEPARATOR)arm-none-eabi-objdump
SIZE    = $(TOOLCHAIN_PATH)$(TOOLCHAIN_SEPARATOR)arm-none-eabi-size

# Flags - Overall Options
CPPFLAGS += -specs=nosys.specs

# Flags - C Language Options
CFLAGS += -ffreestanding
CFLAGS += -Wold-style-cast
CFLAGS += -Wunused-parameter

# Flags - C++ Language Options
CXXFLAGS += -fno-threadsafe-statics
CXXFLAGS += -fno-rtti
CXXFLAGS += -fno-exceptions
CXXFLAGS += -fno-unwind-tables
CXXFLAGS += -Wno-missing-field-initializers

# Flags - Warning Options
CPPFLAGS += -Wall
CPPFLAGS += -Wextra

# Flags - Debugging Options
CPPFLAGS += -g
CPPFLAGS += -gdwarf-2

# Flags - Optimization Options
CPPFLAGS += -ffunction-sections
CPPFLAGS += -fdata-sections
CPPFLAGS += -O2

# Flags - Preprocessor options
CPPFLAGS += -D $(MAPPED_DEVICE)

# Flags - Assembler Options

# Flags - Linker Options
# CPPFLAGS += -nostdlib
CPPFLAGS += -Wl,-L$(BASE_LINKER),-T$(BASE_LINKER)/$(SERIES_FOLDER)/$(DEVICE).ld

# Flags - Directory Options
CPPFLAGS += -I./inc
CPPFLAGS += -I$(BASE_STARTUP)

# Flags - Machine-dependant options
CPPFLAGS += -mcpu=$(SERIES_CPU)
CPPFLAGS += -march=$(SERIES_ARCH)
CPPFLAGS += -mlittle-endian
CPPFLAGS += -mthumb
CPPFLAGS += -masm-syntax-unified


# Output files
ELF_FILE_NAME ?= stm32_executable.elf
BIN_FILE_NAME ?= stm32_bin_image.bin
OBJ_FILE_NAME ?= startup_$(MAPPED_DEVICE).o

ELF_FILE_PATH = $(BIN_FOLDER)/$(ELF_FILE_NAME)
BIN_FILE_PATH = $(BIN_FOLDER)/$(BIN_FILE_NAME)
OBJ_FILE_PATH = $(OBJ_FOLDER)/$(OBJ_FILE_NAME)

# Input files
SRC ?=
SRC += $(SRC_FOLDER)/*.cpp
SRC += $(SRC_FOLDER)/Drivers/*.cpp
SRC += $(SRC_FOLDER)/maths/*.cpp
SRC += $(SRC_FOLDER)/actuators/*.cpp
SRC += $(SRC_FOLDER)/sensors/*.cpp
SRC += $(SRC_FOLDER)/interfaces/*.cpp
SRC += $(SRC_FOLDER)/controllers/*.cpp
SRC += $(SRC_FOLDER)/stm32f4_hal/*.c

CPPFLAGS += -I $(INC_FOLDER)
CPPFLAGS += -I $(INC_FOLDER)/Drivers
CPPFLAGS += -I $(INC_FOLDER)/stm32f4_hal
CPPFLAGS += -I $(INC_FOLDER)/maths
CPPFLAGS += -I $(INC_FOLDER)/actuators
CPPFLAGS += -I $(INC_FOLDER)/controllers
CPPFLAGS += -I $(INC_FOLDER)/interfaces
CPPFLAGS += -I $(INC_FOLDER)/sensors

# Startup file
DEVICE_STARTUP = $(BASE_STARTUP)/$(SERIES_FOLDER)/$(MAPPED_DEVICE).s

# Include the CMSIS files, using the HAL implies using the CMSIS
ifneq (,$(or USE_ST_CMSIS, USE_ST_HAL))
    CPPFLAGS += -D CALL_ARM_SYSTEM_INIT
    CPPFLAGS += -I$(STM32_CUBE_PATH)/CMSIS/ARM/inc
    CPPFLAGS += -I$(STM32_CUBE_PATH)/CMSIS/$(SERIES_FOLDER)/inc

endif

# Include the HAL files
ifdef USE_ST_HAL
    CPPFLAGS += -D USE_HAL_DRIVER
    CPPFLAGS += -I$(STM32_CUBE_PATH)/HAL/$(SERIES_FOLDER)/inc

    # A simply expanded variable is used here to perform the find command only once.
    HAL_SRC := $(shell find $(STM32_CUBE_PATH)/HAL/$(SERIES_FOLDER)/src/*.c ! -name '*_template.c')
    SRC += $(HAL_SRC)
endif

# Make all
all:$(BIN_FILE_PATH)

$(BIN_FILE_PATH): $(ELF_FILE_PATH)
	$(OBJCOPY) -O binary $^ $@

$(ELF_FILE_PATH): $(SRC) $(OBJ_FILE_PATH) | $(BIN_FOLDER)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

$(OBJ_FILE_PATH): $(DEVICE_STARTUP) | $(OBJ_FOLDER)
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

$(BIN_FOLDER):
	mkdir $(BIN_FOLDER)

$(OBJ_FOLDER):
	mkdir $(OBJ_FOLDER)

# Make clean
clean:
	rm -f $(ELF_FILE_PATH)
	rm -f $(BIN_FILE_PATH)
	rm -f $(OBJ_FILE_PATH)

# Make flash
flash:
	st-flash erase
	st-flash write $(BIN_FOLDER)/$(BIN_FILE_NAME) $(FLASH)

.PHONY: all clean flash

# Make erase
erase:
	st-flash erase

# Make Debug
debug:
	openocd -f board/$(OPENOCD_BOARD).cfg
