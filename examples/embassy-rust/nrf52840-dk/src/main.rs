//! Hello world over UARTE0 (P0.06/P0.08) on the nRF52840 DK (Embassy async).

#![no_main]
#![no_std]

use embassy_executor::Spawner;
use embassy_nrf::{bind_interrupts, peripherals, uarte};
use embassy_time::{Duration, Timer};
use panic_halt as _;

// UARTE0 is interrupt-driven (EasyDMA completion), so route its IRQ to embassy-nrf.
bind_interrupts!(struct Irqs {
    UARTE0 => uarte::InterruptHandler<peripherals::UARTE0>;
});

#[embassy_executor::main]
async fn main(_spawner: Spawner) {
    let p = embassy_nrf::init(embassy_nrf::config::Config::default());

    let mut config = uarte::Config::default();
    config.parity = uarte::Parity::Excluded;
    config.baudrate = uarte::Baudrate::Baud115200;

    let mut uart = uarte::Uarte::new(p.UARTE0, p.P0_08, p.P0_06, Irqs, config);

    // EasyDMA needs the source buffer in RAM, so keep the message stack-local.
    let msg: [u8; 13] = *b"Hello world!\n";

    loop {
        uart.write(&msg).await.unwrap();
        Timer::after(Duration::from_secs(1)).await;
    }
}
