# Chiplab

> Simulate & validate firmware on virtual instances of real chips, straight from your AI coding agent — no hardware.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

Chiplab is a hosted MCP service.
Your AI coding agent connects to it, uploads compiled firmware, runs it on a virtual instance of a real microcontroller, and reads back the UART output.

We're trying to build the next way to develop chips — and we're doing it in the open,
early. Today it runs `Hello world!` on STM32 and Nordic boards; there's a lot more on
the way - see **[veecle.ai](https://veecle.ai)** for more infos. Give it a try, poke at it, and tell us what you'd like
to see. We'd genuinely love your feedback, and contributions are very welcome.

## Quickstart

1. Create a free account at **[veecle.ai](https://veecle.ai)** (no credit card).
2. Add the Chiplab MCP server to your agent (see [Connect your agent](#connect-your-agent)).
3. `git clone https://github.com/veecle/chiplab && cd chiplab`
4. One-time: `rustup target add thumbv7m-none-eabi`
5. Tell your agent: *"Build `examples/bare-metal/stm32f4-discovery` and run it on Chiplab."*
6. Read `Hello world!` back from the virtual chip's UART.

## Connect your agent

Chiplab is an HTTP MCP server at `https://chiplab.veecle.ai/mcp`. Add it to any
MCP-compatible client using the standard `mcpServers` schema:

```json
{
  "mcpServers": {
    "chiplab": {
      "type": "http",
      "url": "https://chiplab.veecle.ai/mcp"
    }
  }
}
```

On first use your client opens a browser to sign in. Verify
the connection by asking your agent to call Chiplab's discovery/help tool with no
arguments.

Client-specific notes: Claude Code — if you cloned this repo, the server is already
configured via [`.mcp.json`](.mcp.json); Claude Code will prompt you to trust it.
Otherwise add it with one command —
`claude mcp add --transport http chiplab https://chiplab.veecle.ai/mcp`. Claude Desktop
(`claude_desktop_config.json`) and Cursor (`.cursor/mcp.json`) use the `mcpServers`
block above. VS Code (`.vscode/mcp.json`) uses the same block but under a `servers` key
instead of `mcpServers`. Codex uses TOML in `~/.codex/config.toml` — add
`[mcp_servers.chiplab]` with `url = "https://chiplab.veecle.ai/mcp"`, then run
`codex mcp login chiplab` to sign in.

## Build & run an example

Chiplab's contract is framework-agnostic: **produce an ELF → upload it → run it on your
target board → read the captured output.** How you produce the ELF is
framework-specific — see the [frameworks](#frameworks) below. The Rust hero path:

```sh
rustup target add thumbv7m-none-eabi          # one-time, per target
cd examples/bare-metal/stm32f4-discovery
cargo build --release
# ELF: target/thumbv7m-none-eabi/release/hello-stm32f4-discovery
```

With an ELF in hand, your agent uploads it and runs it on the target board — the same
flow for every board and framework, only the ELF path and board change. The connected
MCP client surfaces the exact tool calls and parameters live; ask Chiplab's discovery/
help tool if you need a refresher.

The run returns synchronously with the captured output — the firmware runs for a
bounded amount of virtual time, and you'll see `Hello world!` in the output.

## Frameworks

Each framework carries its own toolchain, build steps, and coding notes in its directory:

| Framework | Language | Build | Docs |
|---|---|---|---|
| `bare-metal` | Rust (vendor HAL) | `cargo build --release` | [README](examples/bare-metal/README.md) · [AGENTS](examples/bare-metal/AGENTS.md) |
| `embassy-rust` | Rust ([Embassy](https://embassy.dev) async) | `cargo build --release` | [README](examples/embassy-rust/README.md) · [AGENTS](examples/embassy-rust/AGENTS.md) |
| `zephyr-os` | C ([Zephyr RTOS](https://zephyrproject.org)) | `west build` | [README](examples/zephyr-os/README.md) · [AGENTS](examples/zephyr-os/AGENTS.md) |
| `freertos` | C ([FreeRTOS](https://www.freertos.org) kernel) | `make` | [README](examples/freertos/README.md) · [AGENTS](examples/freertos/AGENTS.md) |

See **[supported-boards.md](supported-boards.md)** for every board and which
frameworks cover it.

## Contributing

We'd love your help — and adding an example is the easiest way in. Examples exist to
validate Chiplab end-to-end, so we keep them minimal: one per
`examples/<framework>/<board>/`, named `hello-<board>`, printing `Hello world!` over
UART. Add a board by mirroring an existing example for the same framework and adding a
row to [supported-boards.md](supported-boards.md); each framework's own conventions and
gotchas live in its `examples/<framework>/AGENTS.md`. Full rules — including how to add a
new framework — are in **[AGENTS.md](AGENTS.md)**. New chip/OS/peripheral support is
server-side, so [open a request](https://github.com/veecle/chiplab/issues/new/choose)
rather than adding an example for an unsupported board — and if anything's unclear or
rough, please tell us; we're still smoothing the edges.

## Want a chip, OS, or peripheral we don't cover yet?

Please don't hesitate to ask. Whatever MCU, RTOS/framework, or peripheral you need — or
any other idea — we genuinely want to hear it, and requests really do shape the roadmap.

- **[Open a request or issue](https://github.com/veecle/chiplab/issues/new/choose)** —
  tell us the chip and what you're building.
- Come say hi on [Discord](https://discord.com/invite/F6GwZJ6ktP).

See [veecle.ai/roadmap](https://veecle.ai/roadmap) for what's already planned.

## License

[MIT](LICENSE) © 2026 Veecle GmbH.
