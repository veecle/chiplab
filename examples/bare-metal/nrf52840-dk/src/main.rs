//! Hello world over UARTE0 (P0.06/P0.08) on the nRF52840 DK.

#![no_std]
#![no_main]

use cortex_m_rt::entry;
use nrf52840_hal::{
    gpio::{Level, p0},
    pac,
    uarte::{Baudrate, Parity, Pins, Uarte},
};
use panic_halt as _;

#[entry]
fn main() -> ! {
    let p = pac::Peripherals::take().unwrap();
    let pins = p0::Parts::new(p.P0);

    let uart_pins = Pins {
        txd: pins.p0_06.into_push_pull_output(Level::High).degrade(),
        rxd: pins.p0_08.into_floating_input().degrade(),
        cts: None,
        rts: None,
    };

    let mut uarte = Uarte::new(p.UARTE0, uart_pins, Parity::EXCLUDED, Baudrate::BAUD115200);

    // EasyDMA requires the source buffer to live in RAM, so copy from flash to the stack.
    let message: [u8; 13] = *b"Hello world!\n";
    let _ = uarte.write(&message);

    loop {
        cortex_m::asm::wfi();
    }
}
