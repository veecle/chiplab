# AGENTS.md — Zephyr OS (C)

Framework-specific notes for the `zephyr-os` examples. For the connect → upload → run
contract see the [root AGENTS.md](../../AGENTS.md).

## Toolchain & build

Needs `west` + the Zephyr SDK (plus **Python ≥ 3.12** — Zephyr 4.4 requires it — CMake ≥
3.20, Ninja, `dtc`). Unlike the other frameworks, every board here shares **one west
workspace**, rooted at this directory (`examples/zephyr-os/`): a single `west.yml`
manifest lives here, and each board subfolder commits only its baseline app
(`CMakeLists.txt`, `prj.conf`, `src/main.c`). The user pulls the Zephyr tree, Python
deps, and SDK once with `west`/`pip`; the pulled trees, `build/`, and `.venv/` are
gitignored — never commit them.

This directory (`examples/zephyr-os/`) is the west **manifest repo** (T2 topology),
shared by every board below it — not each board individually:

| Board | west `-b` board | Board key |
|---|---|---|
| stm32f4-discovery | `stm32f4_disco` | `stm32f4_discovery` |
| stm32f7-discovery | `stm32f746g_disco` | `stm32f7_discovery` |
| stm32l073-nucleo | `nucleo_l073rz` | `stm32l073_nucleo` |
| stm32h745-nucleo | `nucleo_h745zi_q/stm32h745xx/m7` | `stm32h745_nucleo` |
| nrf52840-dk | `nrf52840dk/nrf52840` | `nrf52840_dk` |

```sh
cd examples/zephyr-os
python3.13 -m venv .venv && source .venv/bin/activate   # Python ≥ 3.12
pip install west
west init -l .                                          # this dir's west.yml is THE manifest
west update                                             # pulls Zephyr + HAL modules, once
pip install -r zephyr/scripts/requirements-base.txt    # Zephyr's build-time Python deps
west sdk install -t arm-zephyr-eabi                     # the cross-compiler
west build -b <west-board> -d build/<board> <board>    # e.g. -d build/nrf52840-dk nrf52840-dk
# ELF: build/<board>/zephyr/zephyr.elf
```

## Conventions

- Files per board: `CMakeLists.txt`, `prj.conf`, `src/main.c` — no per-board `west.yml`;
  one shared manifest lives at `examples/zephyr-os/west.yml`. One header comment line
  in `main.c` naming the board and console UART.
- `int main(void)` returning `0` (modern Zephyr; `void main()` is deprecated).
  `printf("Hello world!\n")` over `<stdio.h>` reaches the default console.
- Keep `prj.conf` minimal — the board defconfig already enables the UART console; only
  add `CONFIG_*` lines when the board default isn't enough, and say why.
- `find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})` must come **before**
  `project()` in `CMakeLists.txt`.

## Gotchas

- **The workspace is shared, boards are not independently copy-pasteable.** west's T2
  topology roots the workspace one level above wherever `west init -l .` runs; because
  every board dir sits under `examples/zephyr-os/`, only one `west.yml` can ever be the
  active manifest for the whole directory. Adding a board means adding its 3 files and a
  `west -b` table row — never a new `west.yml`. `import: true` in the shared manifest
  already pulls every mainline HAL (`hal_stm32`, `hal_nordic`, …), so no board needs its
  own import. Pin `revision:` to a released tag, not a branch, for reproducible builds.
- **Build each board into its own `-d build/<board>` dir.** Reusing the default
  `build/` for every board clobbers the previous board's build; the ELF is
  `build/<board>/zephyr/zephyr.elf`, not `hello-<board>` — Zephyr fixes the output name.
- **Console must be UART, not RTT.** Chiplab captures UART output; if a board enabled
  `CONFIG_RTT_CONSOLE`/`CONFIG_USE_SEGGER_RTT`, output would bypass `stdout`. The
  nRF52840 DK defaults to `zephyr,console = &uart0`, which Chiplab captures — so
  `prj.conf` stays empty.
- **`west sdk install` needs a workspace** — it's unavailable until after `west init` +
  `west update`, so run it in that order.
- **Python ≥ 3.12, but not bleeding-edge.** Zephyr 4.4 requires ≥ 3.12; some build deps
  (e.g. `jsonschema`) lack wheels for the newest releases. Python 3.13 in a venv is the
  safe choice. The CMake error `Missing jsonschema dependency` means the build-time
  requirements aren't installed in the active interpreter — `pip install -r
  zephyr/scripts/requirements-base.txt`.
- **STM32H745 is dual-core** — target the Cortex-M7 core qualifier
  (`nucleo_h745zi_q/stm32h745xx/m7`); the board's default console (`usart3`) is only
  wired up on that core's defconfig.
- **STM32F103 Blue Pill and STM32WBA52 Nucleo have no upstream Zephyr board yet** —
  mainline Zephyr ships Nucleo boards for other WBA variants (WBA25/55/65) and no
  community port of the generic "Blue Pill" clone board, so these two need a custom
  out-of-tree board port (dts/Kconfig/board.cmake) before an example can exist here.
