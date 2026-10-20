// Chiplab smoke test: the firmware prints "Hello world!" over the captured UART.
import { expect } from "jsr:@std/expect";

Deno.test("firmware greets", async () => {
  const outcome = await simulation.runUntilUartLine("Hello world!", 5000);
  expect(outcome.kind).toBe("matched");
});
