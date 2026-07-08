# Bare-metal (Rust)

`#![no_std]` Rust on a vendor HAL (`stm32f4xx-hal`, `nrf52840-hal`, …), driving the
UART directly — no async runtime. Each example prints `Hello world!` and is the
simplest path to a working ELF.

## Toolchain

Rust ≥ 1.85 (for `edition = "2024"`); `rustup update stable` if needed. Install the
board's target once:

| Board | Rust target |
|---|---|
| stm32f4-discovery | `thumbv7m-none-eabi` |
| stm32f7-discovery | `thumbv7em-none-eabi` |
| stm32f103-blue-pill | `thumbv7m-none-eabi` |
| stm32l073-nucleo | `thumbv6m-none-eabi` |
| stm32h745-nucleo | `thumbv7em-none-eabihf` |
| nrf52840-dk | `thumbv7em-none-eabihf` |

## Build

```sh
rustup target add <rust-target>
cd examples/bare-metal/<board>      # target is pinned in .cargo/config.toml
cargo build --release
# ELF: target/<rust-target>/release/hello-<board>
```

Then upload and run it per the [root README](../../README.md#how-it-works).

Stuck, or want this on a board we don't cover yet? We'd love to hear from you —
[open an issue](https://github.com/veecle/chiplab/issues/new/choose) and we're glad to
help or add boards. Writing a new example? See [AGENTS.md](AGENTS.md) for conventions
and gotchas.
