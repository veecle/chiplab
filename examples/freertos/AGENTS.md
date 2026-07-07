# AGENTS.md — FreeRTOS (C)

Framework-specific notes for the `freertos` examples. For the connect → upload → run
contract see the [root AGENTS.md](../../AGENTS.md).

## Toolchain & build

Needs `arm-none-eabi-gcc` + `make` (macOS: `brew install --cask gcc-arm-embedded`;
Debian/Ubuntu: `apt-get install gcc-arm-none-eabi`). The repo commits only the baseline
app per board (`Makefile`, `src/main.c`, `src/FreeRTOSConfig.h`, `src/startup_<chip>.s`,
`src/linker.ld`); the FreeRTOS kernel is fetched by `make setup` and gitignored, as is
`build/`. Never commit the kernel or build output.

| Board | Chip | FreeRTOS port | Board key |
|---|---|---|---|
| stm32f4-discovery | STM32F407 | `GCC/ARM_CM4F` | `stm32f4_discovery` |

```sh
cd examples/freertos/<board>
make setup     # shallow-clones FreeRTOS-Kernel @ the pinned tag (gitignored)
make           # arm-none-eabi-gcc
# ELF: build/hello-<board>.elf
```

## Conventions

- One header comment line in `main.c` naming the board and UART. No vendor HAL — drive
  the UART with direct register writes (see the bare-metal example for the same pattern).
- Compile: kernel core (`tasks.c`, `list.c`, `queue.c`, `timers.c`), the port
  (`portable/GCC/<port>/port.c`), one heap (`portable/MemMang/heap_4.c`), plus the
  example's `startup_<chip>.s`.
- Keep `configUSE_TIMERS` and other kernel features off unless the example needs them.
- Pin the kernel to a release tag in the `Makefile` (`KERNEL_TAG`), not a branch.

## Gotchas

- **Handler name mapping** — `FreeRTOSConfig.h` must `#define vPortSVCHandler
  SVC_Handler` / `xPortPendSVHandler PendSV_Handler` / `xPortSysTickHandler
  SysTick_Handler`, and the startup vector table must reference those CMSIS names, or the
  scheduler's context switch never fires.
- **Interrupt priorities** — `configMAX_SYSCALL_INTERRUPT_PRIORITY` and
  `configKERNEL_INTERRUPT_PRIORITY` are shifted by `(8 - configPRIO_BITS)` (STM32 = 4
  priority bits). Any ISR using `...FromISR()` APIs must run at a priority numerically
  ≥ the syscall priority.
- **Drain the UART** — block until the TX-complete flag is set before the task yields,
  or the last bytes may not reach `stdout` before the 5 s window closes.
- **Clock** — this example runs on HSI at 16 MHz (no PLL/HSE), matching the bare-metal
  STM32F4 example. `configCPU_CLOCK_HZ` and the USART `BRR` divisor both assume 16 MHz.
- **Tickless idle off** — leave `configUSE_TICKLESS_IDLE` disabled; sleep modes can
  stall the simulation.
- **The ELF is `build/hello-<board>.elf`** — upload that path.
