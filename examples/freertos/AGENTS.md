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
| stm32f7-discovery | STM32F746 | `GCC/ARM_CM7/r0p1` | `stm32f7_discovery` |
| stm32f103-blue-pill | STM32F103 | `GCC/ARM_CM3` | `stm32f103_blue_pill` |
| stm32l073-nucleo | STM32L073 | `GCC/ARM_CM0` | `stm32l073_nucleo` |
| stm32h745-nucleo | STM32H745 | `GCC/ARM_CM7/r0p1` | `stm32h745_nucleo` |
| nrf52840-dk | nRF52840 | `GCC/ARM_CM4F` | `nrf52840_dk` |

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
  (`portable/GCC/<port>/port.c`; the `ARM_CM0` port additionally needs its
  `portasm.c`), one heap (`portable/MemMang/heap_4.c`), plus the example's
  `startup_<chip>.s`.
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
  ≥ the syscall priority. Cortex-M0/M0+ (`GCC/ARM_CM0` port, e.g. stm32l073-nucleo) has
  no `BASEPRI` register, so none of this applies there — critical sections mask all
  interrupts via `PRIMASK` instead, and the port's `SVC_Handler`/`PendSV_Handler`/
  `SysTick_Handler` are already the CMSIS vector-table names, so `FreeRTOSConfig.h`
  needs no handler-name mapping or `configPRIO_BITS` for that port.
- **Drain the UART** — block until the TX-complete flag is set before the task yields,
  or the last bytes may not reach `stdout` before the 5 s window closes.
- **Clock** — the examples run on each chip's reset-default internal oscillator (no
  PLL/HSE), matching the bare-metal examples; `configCPU_CLOCK_HZ` and the USART `BRR`
  divisor are per-board — see each board's `FreeRTOSConfig.h` and `main.c`.
  stm32l073-nucleo is the exception: it boots on MSI and switches to HSI16 in code.
  Chiplab's simulated RCC reflects `HSI16RDY` after `RCC_CR.HSI16ON` but never updates
  `SWS` to match `SW` after the clock-switch write, so poll `HSI16RDY` only and treat
  the `SW` write as fire-and-forget — polling `SWS` to confirm the switch spins forever
  in simulation.
- **STM32H745** is dual-core; this example targets the Cortex-M7 (the
  `GCC/ARM_CM7/r0p1` port). It is a second clock exception: its `configCPU_CLOCK_HZ`
  and USART `BRR` assume 16 MHz like the F4/F7 examples (validated on Chiplab), not
  the chip's 64 MHz reset-default HSI — see the note in its `main.c`.
- **Tickless idle off** — leave `configUSE_TICKLESS_IDLE` disabled; sleep modes can
  stall the simulation.
- **The ELF is `build/hello-<board>.elf`** — upload that path.
