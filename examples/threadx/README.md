# ThreadX (C)

A minimal [Eclipse ThreadX](https://github.com/eclipse-threadx/threadx) application: one thread prints `Hello world!` over the UART, which Chiplab captures into `stdout`.
ThreadX is just a kernel (scheduler + a CPU port), so the example pairs it with a tiny hand-written vendor layer — direct-register UART, startup, low-level kernel hooks, and linker script — instead of a full HAL.

The repo commits only the **baseline app** (`src/`, `Makefile`); the ThreadX kernel is fetched by `make setup` (pinned to a release tag, MIT-licensed) and gitignored, along with the build output.
You provide the toolchain.

## Toolchain

The Arm bare-metal GCC (`arm-none-eabi-gcc`) and `make`:

```sh
# macOS:  brew install --cask gcc-arm-embedded
# Linux:  your distro's gcc-arm-none-eabi package
```

| Board | Chip | ThreadX port | Board key |
|---|---|---|---|
| stm32f4-discovery | STM32F407 | `ports/cortex_m4/gnu` | `stm32f4_discovery` |

## Build

```sh
cd examples/threadx/stm32f4-discovery
make setup     # shallow-clones eclipse-threadx/threadx @ the pinned tag (gitignored)
make           # builds with arm-none-eabi-gcc
# ELF: build/hello-stm32f4-discovery.elf
```

Then upload and run it per the [root README](../../README.md#how-it-works), using that ELF path and the board's key.

Stuck, or want this on a board we don't cover yet?
We'd love to hear from you — [open an issue](https://github.com/veecle/chiplab/issues/new/choose) and we're glad to help or add boards.
Writing a new example?
See [AGENTS.md](AGENTS.md) for conventions and gotchas.
