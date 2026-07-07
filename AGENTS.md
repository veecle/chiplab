# AGENTS.md

Machine-readable guide for AI coding agents working in this repository.

## What is ChipLab

ChipLab simulates firmware on virtual instances of real microcontrollers: upload a
compiled ELF, run it on a named virtual board, read back the captured UART output. (See
the [README](README.md) for the human-facing pitch.)

- **MCP endpoint:** `https://chiplab.veecle.ai/mcp`
- **Account / sign-in:** https://veecle.ai (self-serve; free tier: 50 credits/day, no
  credit card)

This repo ships ready-to-run examples for **every supported board**, grouped by
framework — currently `bare-metal` (vendor-HAL / direct-register), `embassy-rust`
(the [Embassy](https://embassy.dev) async runtime), `zephyr-os` (C on the
[Zephyr RTOS](https://zephyrproject.org), built with `west`), and `freertos` (C on the
[FreeRTOS](https://www.freertos.org) kernel, built with `make`), each documented in
`examples/<framework>/`. A user can open this repo, connect their
agent to ChipLab, and say *"install ChipLab and test it on &lt;board&gt;"* — everything
an agent needs to do that is below.

## 1. Install / connect ChipLab

1. Ensure the user has a veecle.ai account (self-serve sign-up; no manual
   provisioning).
2. Add the ChipLab MCP server to the host agent. Per-client configuration is in the
   [README](README.md#connect-your-agent). The endpoint is
   `https://chiplab.veecle.ai/mcp`; use `chiplab` as the server name.
3. Complete sign-in in the browser when prompted.

## 2. Verify the connection

Call ChipLab's discovery/help tool with no arguments. A successful response returns an
overview of ChipLab. **Always call it first in a session** — it carries chip-specific
gotchas and validated fixes.

## 3. Pick the example for the board you want

Examples live at `examples/<framework>/<board>/`. Do **not** hardcode the board or
framework list here — discover it:

- Call the discovery/help tool for the current set of supported boards and their
  identifiers (it is authoritative and may be ahead of this repo).
- For the board↔framework↔example mapping in this repo, read
  [supported-boards.md](supported-boards.md) — the single source of truth (see its
  `Board key` column).

Map the user's requested board to its example directory and board key, and pick the
framework they asked for (default to `bare-metal`; some boards ship only one framework,
as the matrix shows). If the user doesn't name a board, default to
`bare-metal/stm32f4-discovery` (the hero example).

## 4. Build the example

Build steps are framework-specific. Read the chosen framework's
`examples/<framework>/AGENTS.md` for its toolchain, build command, and ELF path before
building. The Rust frameworks (`bare-metal`, `embassy-rust`) work like this:

```sh
rustup target add <rust-target>     # one-time; target is in examples/<framework>/AGENTS.md
cd examples/<framework>/<board>     # each example pins its target via .cargo/config.toml
cargo build --release
# ELF: target/<rust-target>/release/hello-<board>
```

The Rust frameworks require Rust ≥ 1.85 (for `edition = "2024"`); run `rustup update
stable` if needed. Per-board targets and concrete examples are in each framework's
`examples/<framework>/AGENTS.md`.

## 5. Run it on ChipLab

Upload the ELF, then run it on the target board — the same flow for every board, only
the ELF path and board change. The connected MCP client exposes the exact tool
calls (get an upload slot → upload the bytes → trigger the run) and their parameters
live; call the discovery/help tool if you need the current specifics.

The run returns synchronously. Read the captured output, which contains
`Hello world!` (the `stm32wba52-nucleo` example prints it once per second, so you'll
see several lines).

## Tool reference

ChipLab exposes its tools over standard MCP tool discovery — your MCP client
automatically retrieves the current tool set and their schemas when it connects, so
this doc doesn't hardcode a tool count or exact names (they may change and more may be
added). By convention, ChipLab also provides a discovery/help tool — call it first,
with no arguments — for chip-specific gotchas and validated fixes not captured in the
schemas alone, and to see what else is currently available.

## End-to-end recipe (cold open)

When a user says *"install ChipLab, tell me what to do, and test &lt;board&gt;"*:

1. Confirm/establish the MCP connection (§1) and call the discovery/help tool with no
   arguments (§2).
2. Resolve the board → example dir via
   [supported-boards.md](supported-boards.md) (§3). If none given, use
   `bare-metal/stm32f4-discovery`.
3. Build the ELF per the framework's `examples/<framework>/AGENTS.md` (§4).
4. Upload the ELF, then run it on the board (§5).
5. Report the captured output back to the user — they should see `Hello world!`.

## How to contribute (adding or editing examples)

Examples exist to validate ChipLab end-to-end, nothing more. Keep them minimal.

- **One example per `examples/<framework>/<board>/`.** Mirror an existing example for
  the same framework; the framework's own conventions (layout, deps, gotchas) live in
  `examples/<framework>/AGENTS.md` — read it first.
- **The binary is `hello-<board>`** and prints `Hello world!` over UART, so the ELF
  lands where that framework's doc says (Rust frameworks:
  `target/<rust-target>/release/hello-<board>`).
- **Do not over-comment.** A single `//!`/header line per source file naming the board
  and UART is enough. Only keep a comment when the code is genuinely non-obvious (e.g.
  the nRF EasyDMA-in-RAM requirement, the `--nmagic` linker arg, or a feature-flag
  footgun).
- **No per-example README.** Board metadata lives only in
  [supported-boards.md](supported-boards.md) — add a matrix row there for any new board.
- **Verify** by building per the framework doc, then upload + run it on the
  matching board and confirm `Hello world!` in the captured output.
- **Adding a framework?** Create `examples/<framework>/` with its own `README.md` +
  `AGENTS.md` (toolchain, build, coding notes), add a matrix column in
  [supported-boards.md](supported-boards.md), and link it from the README's Frameworks
  table. New chip/OS/peripheral support is server-side — open a
  [request](https://github.com/veecle/chiplab/issues/new/choose); don't add an example
  for an unsupported board.

## Tips

- Call the discovery/help tool first; it carries chip-specific gotchas and validated
  fixes.
- Cross-compilation happens on your side; ChipLab runs the ELF as-is.
- Simulations are bounded to a fixed amount of virtual time; the captured output is
  the UART capture.
- Boards are supported across the STM32 and Nordic nRF families, each with a
  `bare-metal` and/or `embassy-rust` example. See
  [supported-boards.md](supported-boards.md) for the full board list, and
  call the discovery/help tool for the authoritative, up-to-date set.
- To run your own firmware, build an ELF for the matching target and repeat the
  upload → run steps with the appropriate board.
- Today's verb surface is discovery/help + build-upload-run — call the discovery/help
  tool to confirm what's currently live. Standalone verbs like `build` and `test` are
  planned, not live — don't assume them. See [veecle.ai/roadmap](https://veecle.ai/roadmap).
- Missing a chip, OS, or peripheral?
  [Open a request](https://github.com/veecle/chiplab/issues/new/choose).
