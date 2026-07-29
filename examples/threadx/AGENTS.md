# AGENTS.md — ThreadX (C)

Framework-specific notes for the `threadx` examples.
For the connect → upload → run contract see the [root AGENTS.md](../../AGENTS.md).

## Toolchain & build

Needs `arm-none-eabi-gcc` + `make` (macOS: `brew install --cask gcc-arm-embedded`; Debian/Ubuntu: `apt-get install gcc-arm-none-eabi`).
The repo commits only the baseline app per board (`Makefile`, `src/main.c`, `src/tx_low_level.c`, `src/startup_<chip>.s`, `src/linker.ld`); the ThreadX kernel is fetched by `make setup` and gitignored, as is `build/`.
Never commit the kernel or build output.

| Board | Chip | ThreadX port | Board key |
|---|---|---|---|
| stm32f4-discovery | STM32F407 | `ports/cortex_m4/gnu` | `stm32f4_discovery` |

```sh
cd examples/threadx/<board>
make setup     # shallow-clones eclipse-threadx/threadx @ the pinned tag (gitignored)
make           # arm-none-eabi-gcc
# ELF: build/hello-<board>.elf
```

## Conventions

- One header comment line in `main.c` naming the board and UART.
  No vendor HAL — drive the UART with direct register writes (see the bare-metal example for the same pattern).
- Compile all of `common/src/*.c` (what ThreadX's own CMake build does) plus exactly the port sources listed in the port's `CMakeLists.txt`; `--gc-sections` drops the rest.
  `tx_misra.S` and the standalone `tx_thread_interrupt_disable/restore.S` shims are not part of a normal build.
- `main()` only initialises the UART and calls `tx_kernel_enter()`; objects are created in `tx_application_define()`.
- Thread stacks and control blocks are static arrays, so no byte pool is needed and the `first_unused_memory` argument goes unused.
  Keep it that way unless an example needs dynamic allocation.
- Pin the kernel to a release tag in the `Makefile` (`KERNEL_TAG`), not a branch.
- Clone from `eclipse-threadx/threadx` (MIT), never the archived `azure-rtos/threadx` (whose licence only permitted Microsoft-listed hardware).

## Gotchas

- **The app owns `_tx_initialize_low_level`** — unlike FreeRTOS, the Cortex-M ThreadX port does not ship one for your board.
  It lives in the port's `example_build/` as reference only.
  `src/tx_low_level.c` is this repo's C equivalent: it must set `_tx_initialize_unused_memory` and `_tx_thread_system_stack_ptr`, program `VTOR`, set the exception priorities, and start SysTick.
- **`__RAM_segment_used_end__` is a port-example symbol, not a requirement** — the reference assembly derives first-unused-memory from it.
  Our C version uses the linker script's `_ebss` instead, so the linker script doesn't need that symbol at all.
- **Handler names** — the port defines `PendSV_Handler` (aliased `__tx_PendSVHandler`) in `tx_thread_schedule.S`; the vector table must reference it or the context switch never fires.
  SysTick is the *application's* handler and must call `_tx_timer_interrupt()`.
  SVCall is unused by this port.
- **SVCall and PendSV must be lowest priority** (`0xFF` in SHPR2/SHPR3) — the port relies on the context switch tail-chaining after every other exception.
- **FPU must be enabled in the reset handler** — the build uses the hard-float ABI (`-mfloat-abi=hard -mfpu=fpv4-sp-d16`, matching ThreadX's own build scripts), but ThreadX never touches `CPACR`, so the startup file grants CP10/CP11 access itself.
- **Drain the UART** — block until the TX-complete flag is set before the thread sleeps, or the last bytes may not reach `stdout` before the 5 s window closes.
- **Clock** — this example runs on HSI at 16 MHz (no PLL/HSE), matching the bare-metal and FreeRTOS STM32F4 examples.
  `tx_low_level.c`'s SysTick reload and the USART `BRR` divisor both assume 16 MHz.
  The simulated core runs faster than that, so the observed wall-clock print interval is shorter than `TX_TIMER_TICKS_PER_SECOND` implies — the FreeRTOS example shows the identical skew, so don't "fix" it by retuning the reload.
- **The ELF is `build/hello-<board>.elf`** — upload that path.
