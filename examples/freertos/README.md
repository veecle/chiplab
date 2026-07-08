# FreeRTOS (C)

A minimal [FreeRTOS](https://www.freertos.org) application: one task prints
`Hello world!` over the UART, which Chiplab captures into `stdout`. FreeRTOS is just a
kernel (scheduler + a CPU port), so the example pairs it with a tiny hand-written vendor
layer — direct-register UART, startup, and linker script — instead of a full HAL.

The repo commits only the **baseline app** (`src/`, `Makefile`); the FreeRTOS kernel is
fetched by `make setup` (pinned to a release tag) and gitignored, along with the build
output. You provide the toolchain.

## Toolchain

The Arm bare-metal GCC (`arm-none-eabi-gcc`) and `make`:

```sh
# macOS:  brew install --cask gcc-arm-embedded
# Linux:  your distro's gcc-arm-none-eabi package
```

| Board | Chip | FreeRTOS port | Board key |
|---|---|---|---|
| stm32f4-discovery | STM32F407 | `GCC/ARM_CM4F` | `stm32f4_discovery` |

## Build

```sh
cd examples/freertos/stm32f4-discovery
make setup     # shallow-clones FreeRTOS-Kernel @ the pinned tag (gitignored)
make           # builds with arm-none-eabi-gcc
# ELF: build/hello-stm32f4-discovery.elf
```

Then upload and run it per the [root README](../../README.md#how-it-works),
using that ELF path and the board's key.

Stuck, or want this on a board we don't cover yet? We'd love to hear from you —
[open an issue](https://github.com/veecle/chiplab/issues/new/choose) and we're glad to
help or add boards. Writing a new example? See [AGENTS.md](AGENTS.md) for conventions
and gotchas.
