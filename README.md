# Chiplab

> Simulate & validate firmware on virtual instances of real chips, straight from your AI coding agent — no hardware.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

Chiplab is a hosted MCP service.
Your AI coding agent connects to it, uploads compiled firmware, runs it on a virtual instance of a real microcontroller, and reads back the UART output.
You don't drive the toolchain — your agent does.

We're building the next way to develop chips, in the open and early.
Today it runs `Hello world!` on STM32 and Nordic boards; a lot more is on the way — see **[veecle.ai](https://veecle.ai)**.
Try it, poke at it, and tell us what you'd like to see.

## Quickstart

1. Create a free account at **[veecle.ai](https://veecle.ai)** (no credit card).
2. [Connect your agent](#connect-your-agent) to the Chiplab MCP server.
3. `git clone https://github.com/veecle/chiplab && cd chiplab`, then tell your agent:

   > *"Set up Chiplab and run the stm32f4-discovery example on it."*

Your agent reads [AGENTS.md](AGENTS.md), installs what it needs, builds the firmware, uploads it, runs it on the virtual board, and reports `Hello world!` back from the chip's UART.
That's the whole loop.

## Connect your agent

Chiplab is an HTTP MCP server at `https://chiplab.veecle.ai/mcp`.
Most clients take the standard `mcpServers` schema:

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

On first use your client opens a browser to sign in.
To verify, ask your agent to call Chiplab's discovery/help tool with no arguments.

<details>
<summary><b>Claude Code</b></summary>

If you cloned this repo, the server is already configured via [`.mcp.json`](.mcp.json) — Claude Code will prompt you to trust it.
Otherwise:

```sh
claude mcp add --transport http chiplab https://chiplab.veecle.ai/mcp
```
</details>

<details>
<summary><b>Claude Desktop / Cursor</b></summary>

Use the `mcpServers` block above in `claude_desktop_config.json` or `.cursor/mcp.json`.
</details>

<details>
<summary><b>VS Code</b></summary>

Same block in `.vscode/mcp.json`, but under a `servers` key instead of `mcpServers`.
</details>

<details>
<summary><b>Codex</b></summary>

TOML in `~/.codex/config.toml`:

```toml
[mcp_servers.chiplab]
url = "https://chiplab.veecle.ai/mcp"
```

Then sign in with `codex mcp login chiplab`.
</details>

## How it works

The contract is framework-agnostic: your agent **builds an ELF → uploads it → runs it on the target board → reads the captured UART output**.
Runs return synchronously and are bounded to a fixed amount of virtual time.

This repo ships a ready-to-run example for every supported board, grouped by framework — [`bare-metal`](examples/bare-metal) (Rust, vendor HAL), [`embassy-rust`](examples/embassy-rust) ([Embassy] async), [`zephyr-os`](examples/zephyr-os) ([Zephyr RTOS], C), and [`freertos`](examples/freertos) ([FreeRTOS], C).
The full board × framework matrix is in **[supported-boards.md](supported-boards.md)**.

[Embassy]: https://embassy.dev
[Zephyr RTOS]: https://zephyrproject.org
[FreeRTOS]: https://www.freertos.org

Toolchain and build details live in each framework's directory (`examples/<framework>/README.md` for humans, `AGENTS.md` for agents) — your agent finds them on its own.
Prefer building by hand?
Each framework README has the exact commands.

To run your own firmware, build an ELF for a supported board and ask your agent to upload and run it the same way.

## Contributing

Adding a board example is the easiest way in: mirror an existing example for the same framework and add a row to [supported-boards.md](supported-boards.md).
All conventions live in **[AGENTS.md](AGENTS.md)** and each framework's `AGENTS.md`.
New chip, OS, or peripheral support is server-side — [open a request][new-issue] instead of adding an example for an unsupported board.

[new-issue]: https://github.com/veecle/chiplab/issues/new/choose

## Found a bug? Missing a chip? Tell us here.

This is Chiplab's public home — bugs, feature ideas, and chip/OS/peripheral requests all belong in [this repo's issues][new-issue].
Requests really do shape the roadmap — see [veecle.ai/roadmap](https://veecle.ai/roadmap) for what's planned, or come say hi on [Discord](https://discord.com/invite/F6GwZJ6ktP).

## License

[MIT](LICENSE) © 2026 Veecle GmbH.
