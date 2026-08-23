# ==============================================================================
# Makefile - STM32F407VG Multi-Sensor Fusion Driver Stack
#
# Builds a bare-metal ELF for the STM32F407 (Cortex-M4) using the GNU ARM
# embedded toolchain. Designed to run under Renode (sysbus LoadELF) as well
# as flash to real hardware later (only board_config.h should need to change).
#
# Usage:
#   make            - build firmware.elf (+ .bin, .hex, .map, size report)
#   make clean      - remove build artifacts
#   make flash      - flash to real hardware via st-flash (optional, needs stlink)
# ==============================================================================

# ---- Toolchain ---------------------------------------------------------------
PREFIX      = arm-none-eabi-
CC          = $(PREFIX)gcc
OBJCOPY     = $(PREFIX)objcopy
SIZE        = $(PREFIX)size
GDB         = $(PREFIX)gdb

# ---- Project layout -----------------------------------------------------------
TARGET      = firmware
BUILD_DIR   = build

SRC_DIRS    = src/app src/drivers src/sensors src/fusion src/config
INC_DIRS    = src/app src/drivers src/sensors src/fusion src/config

# Will pick up .c files as they're added to each directory below.
SOURCES     = $(wildcard src/app/*.c) \
              $(wildcard src/drivers/*.c) \
              $(wildcard src/sensors/*.c) \
              $(wildcard src/fusion/*.c)

OBJECTS     = $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SOURCES))
SYSTEM_OBJ  = $(BUILD_DIR)/system_stm32f4xx.o
OBJECTS     += $(SYSTEM_OBJ)
DEPS        = $(OBJECTS:.o=.d)

# ---- MCU / CPU flags (STM32F407VG = Cortex-M4F) --------------------------------
MCU_FLAGS   = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard

# CMSIS is split across two repos - both must be on the include path:
#   - CMSIS-Core (ARM CMSIS_5 repo):  core_cm4.h, cmsis_gcc.h, etc.
#   - CMSIS-Device (ST STM32CubeF4):  stm32f407xx.h, system_stm32f4xx.h
CMSIS_CORE_INC   = CMSIS/Core/Include
CMSIS_DEVICE_INC = CMSIS/Device/ST/STM32F4xx/Include
INC_DIRS         += $(CMSIS_CORE_INC) $(CMSIS_DEVICE_INC)

# system_stm32f4xx.c provides SystemInit() (clock setup), called from the
# startup file before main() - it's a CMSIS-Device source file, not one of
# yours, so it's added directly rather than via the src/ wildcard globs.
SYSTEM_SRC  = CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c

STARTUP     = src/config/startup_stm32f407xx.s
LDSCRIPT    = src/config/STM32F407VGTX_FLASH.ld

INCLUDES    = $(addprefix -I,$(INC_DIRS))

# STM32F407xx tells CMSIS's stm32f4xx.h umbrella header which device header
# to pull in (stm32f407xx.h) - without this it hits a #error and none of
# RCC/SCB/GPIO/etc. (or even stdint types via core_cm4.h) get declared.
DEVICE_DEFINE = -DSTM32F407xx

CFLAGS      = $(MCU_FLAGS) -std=c11 -Wall -Wextra -Wshadow -Wdouble-promotion \
              -fno-common -ffunction-sections -fdata-sections \
              -O0 -g3 -MMD -MP $(DEVICE_DEFINE) $(INCLUDES)

LDFLAGS     = $(MCU_FLAGS) -T$(LDSCRIPT) -Wl,--gc-sections -Wl,-Map=$(BUILD_DIR)/$(TARGET).map \
              --specs=nano.specs --specs=nosys.specs

# ---- Rules ---------------------------------------------------------------------
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).bin $(BUILD_DIR)/$(TARGET).hex size

$(BUILD_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(SYSTEM_OBJ): $(SYSTEM_SRC)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) $(STARTUP)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(MCU_FLAGS) $(STARTUP) $(OBJECTS) $(LDFLAGS) -o $@

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

size: $(BUILD_DIR)/$(TARGET).elf
	$(SIZE) $<

clean:
	rm -rf $(BUILD_DIR)

# Optional: real hardware flashing via st-link (not needed for Renode workflow)
flash: $(BUILD_DIR)/$(TARGET).bin
	st-flash write $< 0x8000000

# Optional: attach GDB to Renode's GDB server (Renode: `machine StartGdbServer 3333`)
debug: $(BUILD_DIR)/$(TARGET).elf
	$(GDB) -ex "target remote localhost:3333" $<

-include $(DEPS)

.PHONY: all clean flash debug size