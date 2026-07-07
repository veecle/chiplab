//! Hello world over USART1 (PB12/PA8) on the STM32WBA52 Nucleo (Embassy async).

#![no_main]
#![no_std]

use embassy_executor::Spawner;
use embassy_stm32::usart::{Config as UartConfig, UartTx};
use embassy_time::{Duration, Timer};
use panic_halt as _;

#[embassy_executor::main]
async fn main(_spawner: Spawner) {
    let p = embassy_stm32::init(embassy_stm32::Config::default());
    let mut tx = UartTx::new_blocking(p.USART1, p.PB12, UartConfig::default()).unwrap();

    loop {
        tx.blocking_write(b"Hello world!\n").unwrap();
        Timer::after(Duration::from_secs(1)).await;
    }
}
