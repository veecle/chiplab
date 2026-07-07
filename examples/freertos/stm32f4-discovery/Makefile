# FreeRTOS hello-world on STM32F4 Discovery, built with arm-none-eabi-gcc.
# `make setup` shallow-clones the FreeRTOS kernel (gitignored); `make` builds the ELF.

PROJECT      := hello-stm32f4-discovery
CROSS        := arm-none-eabi-
CC           := $(CROSS)gcc
SIZE         := $(CROSS)size

KERNEL_REPO  := https://github.com/FreeRTOS/FreeRTOS-Kernel.git
KERNEL_TAG   := V11.3.0
KERNEL_DIR   := freertos-kernel
PORT_DIR     := $(KERNEL_DIR)/portable/GCC/ARM_CM4F

BUILD_DIR    := build
ELF          := $(BUILD_DIR)/$(PROJECT).elf

ARCH_FLAGS   := -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS       := $(ARCH_FLAGS) -Os -g3 -ffunction-sections -fdata-sections -Wall -Wextra
CFLAGS       += -Isrc -I$(KERNEL_DIR)/include -I$(PORT_DIR)
LDFLAGS      := $(ARCH_FLAGS) -Tsrc/linker.ld -nostartfiles -Wl,--gc-sections \
                -Wl,-Map=$(BUILD_DIR)/$(PROJECT).map --specs=nano.specs --specs=nosys.specs

C_SOURCES    := src/main.c \
                $(KERNEL_DIR)/tasks.c \
                $(KERNEL_DIR)/list.c \
                $(KERNEL_DIR)/queue.c \
                $(PORT_DIR)/port.c \
                $(KERNEL_DIR)/portable/MemMang/heap_4.c
ASM_SOURCES  := src/startup_stm32f407.s

OBJECTS      := $(addprefix $(BUILD_DIR)/,$(C_SOURCES:.c=.o) $(ASM_SOURCES:.s=.o))

.PHONY: all setup clean

all: $(ELF)

# One-time: fetch the kernel pinned to a release tag. Gitignored; never committed.
setup:
	@test -d $(KERNEL_DIR) || git clone --depth 1 --branch $(KERNEL_TAG) $(KERNEL_REPO) $(KERNEL_DIR)

$(ELF): $(OBJECTS) | check-kernel
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SIZE) $@

$(BUILD_DIR)/%.o: %.c | check-kernel
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(CC) $(ARCH_FLAGS) -c $< -o $@

check-kernel:
	@test -d $(KERNEL_DIR) || { echo "FreeRTOS kernel missing — run 'make setup' first."; exit 1; }

clean:
	rm -rf $(BUILD_DIR)
