# Zephyr OS

[Zephyr RTOS](https://zephyrproject.org) C firmware. Each example prints `Hello world!`
over the board's default Zephyr console (UART), which Chiplab captures into `stdout`.

Unlike the Rust frameworks, a Zephyr build needs a **west workspace**: the Zephyr tree
plus its HAL modules, pulled by `west`. This repo commits only the **baseline
application** for each board — `west.yml`, `CMakeLists.txt`, `prj.conf`, `src/main.c`.
You provide the toolchain and pull the tree yourself with `west`; the pulled trees and
build output are gitignored.

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
pip install -r ../zephyr/scripts/requirements-base.txt   # from the example dir
west sdk install -t arm-zephyr-eabi                       # the cross-compiler
```

`west sdk install` only works once a workspace exists (after `west init` + `west
update`).

| Board | west `-b` board | Board key |
|---|---|---|
| nrf52840-dk | `nrf52840dk/nrf52840` | `nrf52840_dk` |

## Build

Each example folder is its own west **manifest repo** (T2 topology), so you initialise
the workspace in place, pull the tree, install the SDK, then build:

```sh
cd examples/zephyr-os/nrf52840-dk
python3.13 -m venv .venv && source .venv/bin/activate   # Python ≥ 3.12
pip install west
west init -l .                                          # this folder's west.yml is the manifest
west update                                             # pulls Zephyr + HAL modules (network; minutes)
pip install -r ../zephyr/scripts/requirements-base.txt  # Zephyr's build-time Python deps
west sdk install -t arm-zephyr-eabi                     # the cross-compiler
west build -b nrf52840dk/nrf52840
# ELF: build/zephyr/zephyr.elf
```

`west update`, `west sdk install`, and `west build` create `zephyr/`, `modules/`,
`.west/`, and `build/` — all gitignored. Re-running `west build` is incremental.

Then upload and run it per the [root README](../../README.md#build--run-an-example),
using the `zephyr.elf` path and the board's key.

Stuck, or want this on a board we don't cover yet? We'd love to hear from you —
[open an issue](https://github.com/veecle/chiplab/issues/new/choose) and we're glad to
help or add boards. Writing a new example? See [AGENTS.md](AGENTS.md) for conventions
and gotchas.
