# ThreadX hello-world on STM32F4 Discovery, built with arm-none-eabi-gcc.
# `make setup` shallow-clones the ThreadX kernel (gitignored); `make` builds the ELF.

PROJECT      := hello-stm32f4-discovery
CROSS        := arm-none-eabi-
CC           := $(CROSS)gcc
SIZE         := $(CROSS)size

KERNEL_REPO  := https://github.com/eclipse-threadx/threadx.git
KERNEL_TAG   := v6.5.1.202602a_rel
KERNEL_DIR   := threadx-kernel
PORT_DIR     := $(KERNEL_DIR)/ports/cortex_m4/gnu

BUILD_DIR    := build
ELF          := $(BUILD_DIR)/$(PROJECT).elf

ARCH_FLAGS   := -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16
INCLUDES     := -Isrc -I$(KERNEL_DIR)/common/inc -I$(PORT_DIR)/inc
CFLAGS       := $(ARCH_FLAGS) $(INCLUDES) -Os -g3 -ffunction-sections -fdata-sections \
                -Wall -Wextra
LDFLAGS      := $(ARCH_FLAGS) -Tsrc/linker.ld -nostartfiles -Wl,--gc-sections \
                -Wl,-Map=$(BUILD_DIR)/$(PROJECT).map --specs=nano.specs --specs=nosys.specs

# The whole kernel core, as ThreadX's own CMake build does; --gc-sections drops
# whatever this example doesn't call.
C_SOURCES    := src/main.c \
                src/tx_low_level.c \
                $(wildcard $(KERNEL_DIR)/common/src/*.c)

# Exactly the port sources ThreadX's CMakeLists lists (tx_misra.S and the standalone
# interrupt disable/restore shims are not part of a normal build).
ASM_SOURCES  := src/startup_stm32f407.s \
                $(PORT_DIR)/src/tx_thread_context_restore.S \
                $(PORT_DIR)/src/tx_thread_context_save.S \
                $(PORT_DIR)/src/tx_thread_interrupt_control.S \
                $(PORT_DIR)/src/tx_thread_schedule.S \
                $(PORT_DIR)/src/tx_thread_stack_build.S \
                $(PORT_DIR)/src/tx_thread_system_return.S \
                $(PORT_DIR)/src/tx_timer_interrupt.S

OBJECTS      := $(addprefix $(BUILD_DIR)/,$(C_SOURCES:.c=.o) \
                $(patsubst %.S,%.o,$(patsubst %.s,%.o,$(ASM_SOURCES))))

.PHONY: all setup clean check-kernel

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

$(BUILD_DIR)/%.o: %.S | check-kernel
	@mkdir -p $(dir $@)
	$(CC) $(ARCH_FLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(CC) $(ARCH_FLAGS) -c $< -o $@

# The second test catches `make setup all`: C_SOURCES is wildcarded at parse time, so a
# kernel cloned by an earlier target in the same run contributes no objects to the link.
check-kernel:
	@test -d $(KERNEL_DIR) || { echo "ThreadX kernel missing — run 'make setup' first."; exit 1; }
	@test -n "$(filter $(KERNEL_DIR)/%,$(C_SOURCES))" || { echo "Kernel was fetched after this make started — run 'make' again."; exit 1; }

clean:
	rm -rf $(BUILD_DIR)
