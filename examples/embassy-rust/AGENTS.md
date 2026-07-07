# AGENTS.md — Embassy (Rust)

Framework-specific notes for the `embassy-rust` examples. For the connect → upload →
run contract see the [root AGENTS.md](../../AGENTS.md).

## Toolchain & build

Rust ≥ 1.85 (`edition = "2024"`). Each example pins its target via `.cargo/config.toml`,
so `cargo build --release` in the example dir is enough; install the target once.

| Board | Rust target |
|---|---|
| stm32f4-discovery | `thumbv7m-none-eabi` |
| stm32f7-discovery | `thumbv7em-none-eabi` |
| stm32f103-blue-pill | `thumbv7m-none-eabi` |
| stm32l073-nucleo | `thumbv6m-none-eabi` |
| stm32wba52-nucleo | `thumbv8m.main-none-eabihf` |
| nrf52840-dk | `thumbv7em-none-eabihf` |

```sh
rustup target add <rust-target>
cd examples/embassy-rust/<board>
cargo build --release
# ELF: target/<rust-target>/release/hello-<board>
```

## Conventions

- Files: `Cargo.toml`, `.cargo/config.toml`, `build.rs`, `src/main.rs` (+ `memory.x` on
  nRF). Binary `hello-<board>`; one `//!` header line in `main.rs`.
- `#![no_std]` + `#![no_main]`, `#[embassy_executor::main]` async entry, `panic-halt`.
  `embassy-executor` features: `platform-cortex-m` + `executor-thread`.

## Gotchas

- **`cortex-m-rt` is a direct dependency** — `#[embassy_executor::main]` expands to
  `cortex_m_rt::entry`.
- **STM32**: `embassy-stm32` with the chip feature + `memory-x` (auto-generates
  `memory.x`), `time-driver-any`, `time`.
- **nRF52840**: `embassy-nrf` interrupt-driven `uarte`; the TX buffer must live in RAM —
  EasyDMA can't read from flash, so keep the message stack-local.
- **STM32WBA52**: do *not* enable `embassy-stm32`'s `executor-thread` alongside
  `embassy-executor`'s — duplicate `__pender` symbol.
