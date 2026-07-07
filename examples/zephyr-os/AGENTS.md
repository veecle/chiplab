# AGENTS.md — Zephyr OS (C)

Framework-specific notes for the `zephyr-os` examples. For the connect → upload → run
contract see the [root AGENTS.md](../../AGENTS.md).

## Toolchain & build

Needs `west` + the Zephyr SDK (plus **Python ≥ 3.12** — Zephyr 4.4 requires it — CMake ≥
3.20, Ninja, `dtc`). The repo commits only the baseline app per board (`west.yml`,
`CMakeLists.txt`, `prj.conf`, `src/main.c`); the user pulls the Zephyr tree, Python deps,
and SDK themselves with `west`/`pip`. The pulled trees, `build/`, and `.venv/` are
gitignored — never commit them.

Each example folder is its own west **manifest repo** (T2 topology):

| Board | west `-b` board | Board key |
|---|---|---|
| nrf52840-dk | `nrf52840dk/nrf52840` | `nrf52840_dk` |

```sh
cd examples/zephyr-os/<board>
python3.13 -m venv .venv && source .venv/bin/activate   # Python ≥ 3.12
pip install west
west init -l .                                          # this folder's west.yml is the manifest
west update                                             # pulls Zephyr + HAL modules
pip install -r ../zephyr/scripts/requirements-base.txt  # Zephyr's build-time Python deps
west sdk install -t arm-zephyr-eabi                     # the cross-compiler
west build -b <west-board>                              # the west `-b` board column above
# ELF: build/zephyr/zephyr.elf
```

## Conventions

- Files: `west.yml`, `CMakeLists.txt`, `prj.conf`, `src/main.c`. One header comment line
  in `main.c` naming the board and console UART.
- `int main(void)` returning `0` (modern Zephyr; `void main()` is deprecated).
  `printf("Hello world!\n")` over `<stdio.h>` reaches the default console.
- Keep `prj.conf` minimal — the board defconfig already enables the UART console; only
  add `CONFIG_*` lines when the board default isn't enough, and say why.
- `find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})` must come **before**
  `project()` in `CMakeLists.txt`.

## Gotchas

- **`west.yml` is T2** — `self: path: .` makes the example dir the manifest repo, and
  `import: true` pulls the board's HAL module (e.g. `hal_nordic`). Pin `revision:` to a
  released tag, not a branch, for reproducible builds.
- **The ELF is `build/zephyr/zephyr.elf`** — not `hello-<board>`; Zephyr fixes the
  output name. Upload that path.
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
  ../zephyr/scripts/requirements-base.txt`.
