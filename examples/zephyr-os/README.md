# Zephyr OS

[Zephyr RTOS](https://zephyrproject.org) C firmware. Each example prints `Hello world!`
over the board's default Zephyr console (UART), which Chiplab captures into `stdout`.

Unlike the Rust frameworks, a Zephyr build needs a **west workspace**: the Zephyr tree
plus its HAL modules, pulled by `west`. Every board shares **one workspace**, rooted at
this directory (`examples/zephyr-os/`) — this repo commits one shared `west.yml` here
plus, per board, only the **baseline application** (`CMakeLists.txt`, `prj.conf`,
`src/main.c`). You provide the toolchain and pull the tree yourself with `west`; the
pulled trees and build output are gitignored.

## Toolchain

You need `west`, the Zephyr SDK, **Python ≥ 3.12** (Zephyr 4.4 requires it), CMake ≥
3.20, Ninja, and the devicetree compiler (`dtc`) — see the
[Zephyr getting-started guide](https://docs.zephyrproject.org/latest/develop/getting_started/index.html).
Work inside a Python virtual environment so Zephyr's build dependencies don't collide
with the system interpreter:

```sh
python3.13 -m venv .venv && source .venv/bin/activate   # Python ≥ 3.12
pip install west
```

After `west update` (below), install Zephyr's Python requirements and the SDK into that
same environment:

```sh
pip install -r zephyr/scripts/requirements-base.txt   # from examples/zephyr-os/
west sdk install -t arm-zephyr-eabi                     # the cross-compiler
```

`west sdk install` only works once a workspace exists (after `west init` + `west
update`).

| Board | west `-b` board | Board key |
|---|---|---|
| stm32f4-discovery | `stm32f4_disco` | `stm32f4_discovery` |
| stm32f7-discovery | `stm32f746g_disco` | `stm32f7_discovery` |
| stm32l073-nucleo | `nucleo_l073rz` | `stm32l073_nucleo` |
| stm32h745-nucleo | `nucleo_h745zi_q/stm32h745xx/m7` | `stm32h745_nucleo` |
| nrf52840-dk | `nrf52840dk/nrf52840` | `nrf52840_dk` |

## Build

Every board shares one west **manifest repo** (T2 topology) rooted at this directory,
so you initialise the workspace here once, pull the tree, install the SDK, then build
each board into its own build dir:

```sh
cd examples/zephyr-os
python3.13 -m venv .venv && source .venv/bin/activate   # Python ≥ 3.12
pip install west
west init -l .                                          # this dir's west.yml is THE manifest
west update                                             # pulls Zephyr + HAL modules (network; minutes)
pip install -r zephyr/scripts/requirements-base.txt    # Zephyr's build-time Python deps
west sdk install -t arm-zephyr-eabi                     # the cross-compiler
west build -b nrf52840dk/nrf52840 -d build/nrf52840-dk nrf52840-dk
# ELF: build/nrf52840-dk/zephyr/zephyr.elf
```

`west update`, `west sdk install`, and `west build` create `zephyr/`, `modules/`,
`.west/`, and `build/` — all gitignored, shared by every board. Build a different board
with its own `-d build/<board>` so builds don't clobber each other; re-running
`west build` for a given board is incremental.

Then upload and run it per the [root README](../../README.md#how-it-works),
using the `zephyr.elf` path and the board's key.

Stuck, or want this on a board we don't cover yet? We'd love to hear from you —
[open an issue](https://github.com/veecle/chiplab/issues/new/choose) and we're glad to
help or add boards. Writing a new example? See [AGENTS.md](AGENTS.md) for conventions
and gotchas.
