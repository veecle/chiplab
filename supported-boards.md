# Supported boards, OS/frameworks & examples

This is the single source of truth for what Chiplab supports **today** — and the list
keeps growing. The `Board key` column is this repo's identifier for each board; call
the discovery/help tool to confirm the exact value and format your MCP client should
pass. Each board has one or more ready-to-run examples under
[`examples/`](examples), one per OS/framework.

OS/frameworks available today:

- **`bare-metal`** — vendor-HAL / direct-register Rust firmware (no async runtime).
- **`embassy-rust`** — the same `Hello world!` on the [Embassy](https://embassy.dev)
  async runtime.
- **`zephyr-os`** — the same `Hello world!` on the [Zephyr RTOS](https://zephyrproject.org)
  (C, built with `west`).
- **`freertos`** — the same `Hello world!` on the [FreeRTOS](https://www.freertos.org)
  kernel (C, built with `make` + `arm-none-eabi-gcc`).

The matrix below lists every board, its board key, and the example that exists for
each framework (— = not available yet). Toolchain and target details (e.g. the Rust
target per board) live in each framework's doc under
[`examples/<framework>/`](examples).

| Board | Chip | Family | Board key | `bare-metal` | `embassy-rust` | `zephyr-os` | `freertos` |
|---|---|---|---|---|---|---|---|
| STM32F4 Discovery | STM32F407 | STM32F4 | `stm32f4_discovery` | [example](examples/bare-metal/stm32f4-discovery) | [example](examples/embassy-rust/stm32f4-discovery) | — | [example](examples/freertos/stm32f4-discovery) |
| STM32F7 Discovery | STM32F746 | STM32F7 | `stm32f7_discovery` | [example](examples/bare-metal/stm32f7-discovery) | [example](examples/embassy-rust/stm32f7-discovery) | — | — |
| STM32F103 Blue Pill | STM32F103 | STM32F1 | `stm32f103_blue_pill` | [example](examples/bare-metal/stm32f103-blue-pill) | [example](examples/embassy-rust/stm32f103-blue-pill) | — | — |
| STM32WBA52 Nucleo | STM32WBA52 | STM32WBA | `stm32wba52_nucleo` | — | [example](examples/embassy-rust/stm32wba52-nucleo) | — | — |
| STM32L073 Nucleo | STM32L073 | STM32L0 | `stm32l073_nucleo` | [example](examples/bare-metal/stm32l073-nucleo) | [example](examples/embassy-rust/stm32l073-nucleo) | — | — |
| STM32H745 Nucleo | STM32H745 | STM32H7 | `stm32h745_nucleo` | [example](examples/bare-metal/stm32h745-nucleo) | — | — | — |
| nRF52840 DK | nRF52840 | nRF52 | `nrf52840_dk` | [example](examples/bare-metal/nrf52840-dk) | [example](examples/embassy-rust/nrf52840-dk) | [example](examples/zephyr-os/nrf52840-dk) | — |

## What simulation covers

Chiplab boots a virtual instance of the chip using board-level platform descriptions,
executed on an open simulation platform, with more platforms joining as coverage
grows. Each run includes:

- CPU execution of your ELF binary.
- UART/USART/LPUART peripherals. All output is captured and returned in the run's
  output.
- Peripheral registers and interrupt timing matching the physical chip.

Runs are bounded to a fixed amount of virtual CPU time (not wall-clock time).

## Roadmap

More chips — and more verbs (`build`, `test`, and beyond) — are coming. See
[veecle.ai/roadmap](https://veecle.ai/roadmap) for the capability and chip-family
timeline.

Don't see your chip? Please don't hesitate to ask — we genuinely want to hear it, and
requests really do shape what we add next.
**[Open a request](https://github.com/veecle/chiplab/issues/new/choose)** with your
target MCU and use case, or come say hi on
[Discord](https://discord.com/invite/F6GwZJ6ktP).
