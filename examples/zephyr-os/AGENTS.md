# AGENTS.md — Zephyr OS (C)

Framework-specific notes for the `zephyr-os` examples.
For the connect → upload → run contract see the [root AGENTS.md](../../AGENTS.md).

## Toolchain & build

Needs `west` + the Zephyr SDK (plus **Python ≥ 3.12** — Zephyr 4.4 requires it — CMake ≥ 3.20, Ninja, `dtc`).
Unlike the other frameworks, every board here shares **one west workspace** whose manifest repo is this directory (`examples/zephyr-os/`): a single `west.yml` manifest lives here, and each board subfolder commits only its baseline app (`CMakeLists.txt`, `prj.conf`, `src/main.c`).
Three separate steps populate that workspace: `west update` pulls the Zephyr tree and the HAL modules into this directory, `pip` installs Zephyr's build-time Python dependencies into the active virtualenv, and `west sdk install` unpacks the cross-compiler into the user's home directory (`~/zephyr-sdk-<version>/`, outside this repo).
The pulled trees, `build/`, and `.venv/` are gitignored — never commit them.

This directory (`examples/zephyr-os/`) is the west **manifest repo** (T2 topology), shared by every board below it — not each board individually:

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
west update --narrow                                    # Zephyr + HAL modules, pinned revision only
pip install -r zephyr-src/scripts/requirements-base.txt # Zephyr's build-time Python deps
west sdk install -t arm-zephyr-eabi                     # cross-compiler -> ~/zephyr-sdk-<version>/
west build -b <west-board> -d build/<board> <board>     # e.g. -d build/nrf52840-dk nrf52840-dk
# ELF: build/<board>/zephyr/zephyr.elf
```

Append `-o=--depth=1` to the `west update` line to skip history as well, taking roughly a third off the workspace size — see the shallow-fetch gotcha below for the tradeoffs.

## Conventions

- Files per board: `CMakeLists.txt`, `prj.conf`, `src/main.c` — no per-board `west.yml`; one shared manifest lives at `examples/zephyr-os/west.yml`.
  One header comment line in `main.c` naming the board and console UART.
- `int main(void)` returning `0` (modern Zephyr; `void main()` is deprecated).
  `printf("Hello world!\n")` over `<stdio.h>` reaches the default console.
- Keep `prj.conf` minimal — the board defconfig already enables the UART console; only add `CONFIG_*` lines when the board default isn't enough, and say why.
- `find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})` must come **before** `project()` in `CMakeLists.txt`.

## Gotchas

- **The workspace is shared, boards are not independently copy-pasteable.**
  west's T2 topdir (`.west/`) roots one level above wherever `west init -l .` runs, but the manifest's `import: path-prefix: zephyr-os` nests the pulled `zephyr-src/` and `modules/` trees back under this directory (`examples/zephyr-os/`) instead of leaving them in `examples/`.
  Every board dir sits under `examples/zephyr-os/`, so only one `west.yml` can ever be the active manifest for the whole directory.
  Adding a board means adding its 3 files, a `west -b` table row, and an entry in the `boards` list of [`.github/workflows/build-examples.yml`](../../.github/workflows/build-examples.yml) (CI fails on an unmapped board) — never a new `west.yml`.
  The shared manifest's `import: name-allowlist` pulls only the HAL modules the boards here actually link (`cmsis`, `cmsis_6`, `hal_stm32`, `hal_nordic`) instead of every mainline HAL — a new board needs a new allowlist entry only if it needs a HAL not already listed.
  Pin `revision:` to a released tag, not a branch, for reproducible builds.
- **The zephyr checkout is named `zephyr-src`, not `zephyr`.**
  West's own module auto-detection (`zephyr_module.py`) treats any directory that directly contains both `zephyr/CMakeLists.txt` and `zephyr/Kconfig` as an unmarked module.
  Since the checkout nests inside this self/manifest repo, a literal `zephyr` leaf name would make `examples/zephyr-os/` itself match that heuristic (its own `zephyr/` subdir *is* Zephyr's top-level `CMakeLists.txt` + `Kconfig`), registering a phantom module whose Kconfig re-sources `Kconfig.zephyr` — an infinite `recursive 'source'` CMake error.
  The explicit `path: zephyr-src` on the `zephyr` project in `west.yml` avoids this; don't rename it back to `zephyr` without re-checking this collision.
- **`--narrow` limits refs, not history — `-o=--depth=1` limits history.**
  `west update --narrow` fetches each project's pinned revision instead of every branch plus `--tags`, but the full history reachable from that revision still comes down.
  Adding `-o=--depth=1` fetches a single commit per project, taking the pulled trees from ≈2.8 GB to ≈1.9 GB (the `.git` directories alone shrink from ≈1.1 GB to ≈190 MB).
  Builds are unaffected — the checked-out files are the same revision either way — and so is the boot banner, because `--narrow` already skips tags, so `git describe` prints a bare SHA with or without the flag.
  The `=` is required: `west update -o --depth=1` fails with `error: argument -o/--fetch-opt: expected one argument`.
  Two costs to know about: `git bisect` and `git blame` inside a pulled tree need `git -C zephyr-src fetch --unshallow` first, and shallow-fetching a SHA revision isn't supported by every Git host — if a fetch fails, drop the flag.
- **Build each board into its own `-d build/<board>` dir.**
  Reusing the default `build/` for every board clobbers the previous board's build; the ELF is `build/<board>/zephyr/zephyr.elf`, not `hello-<board>` — Zephyr fixes the output name.
- **Console must be UART, not RTT.**
  Chiplab captures UART output; if a board enabled `CONFIG_RTT_CONSOLE`/`CONFIG_USE_SEGGER_RTT`, output would bypass `stdout`.
  The nRF52840 DK defaults to `zephyr,console = &uart0`, which Chiplab captures — so `prj.conf` stays empty.
- **`west sdk install` needs a workspace** — it's unavailable until after `west init` + `west update`, so run it in that order.
- **Python ≥ 3.12, but not bleeding-edge.**
  Zephyr 4.4 requires ≥ 3.12; some build deps (e.g. `jsonschema`) lack wheels for the newest releases.
  Python 3.13 in a venv is the safe choice.
  The CMake error `Missing jsonschema dependency` means the build-time requirements aren't installed in the active interpreter — `pip install -r zephyr-src/scripts/requirements-base.txt`.
- **STM32H745 is dual-core** — target the Cortex-M7 core qualifier (`nucleo_h745zi_q/stm32h745xx/m7`); the board's default console (`usart3`) is only wired up on that core's defconfig.
- **STM32F103 Blue Pill and STM32WBA52 Nucleo have no upstream Zephyr board yet** — mainline Zephyr ships Nucleo boards for other WBA variants (WBA25/55/65) and no community port of the generic "Blue Pill" clone board, so these two need a custom out-of-tree board port (dts/Kconfig/board.cmake) before an example can exist here.
