# AGENTS.md — bare-metal (Rust)

Framework-specific notes for the `bare-metal` examples. For the connect → upload → run
contract see the [root AGENTS.md](../../AGENTS.md).

## Toolchain & build

`rustup toolchain install` installs the toolchain and targets pinned in
[rust-toolchain.toml](../../rust-toolchain.toml), and each example pins its target via
`.cargo/config.toml`, so `cargo build --release` in the example dir is enough. A new
example's target must be added to `rust-toolchain.toml`.

| Board | Rust target |
|---|---|
| stm32f4-discovery | `thumbv7m-none-eabi` |
| stm32f7-discovery | `thumbv7em-none-eabi` |
| stm32f103-blue-pill | `thumbv7m-none-eabi` |
| stm32l073-nucleo | `thumbv6m-none-eabi` |
| stm32h753-nucleo | `thumbv7em-none-eabihf` |
| nrf52840-dk | `thumbv7em-none-eabihf` |

```sh
rustup toolchain install
cd examples/bare-metal/<board>
cargo build --release
# ELF: target/<rust-target>/release/hello-<board>
```

With Nix, `nix develop` provides a toolchain with every target, and `nix build .#bare-metal-<board>` from the repo root builds the ELF to `result/hello-<board>.elf`.

## Conventions

- Files: `Cargo.toml`, `.cargo/config.toml`, `build.rs`, `memory.x`, `src/main.rs`.
  Binary `hello-<board>`; one `//!` header line in `main.rs`.
- `#![no_std]` + `#![no_main]`, `cortex_m_rt::entry`, `panic-halt`. Use the vendor HAL,
  configure clocks, write bytes over the UART, then `loop {}`.

## Gotchas

- **`build.rs`** writes `memory.x` and links `-Tlink.x`; keep `--nmagic` — required when
  memory sections aren't aligned to `0x10000`
  ([cortex-m-quickstart#95](https://github.com/rust-embedded/cortex-m-quickstart/pull/95)).
- **nRF52840** (`nrf52840-hal`): the UARTE TX buffer must live in RAM — EasyDMA can't
  read from flash, so copy the message to the stack.
