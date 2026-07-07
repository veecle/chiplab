//! Hello world over USART2 (PA2/PA3) on the STM32L073 Nucleo.

#![no_main]
#![no_std]

use cortex_m_rt::entry;
use nb::block;
use panic_halt as _;
// stm32l0xx-hal 0.10 needs embedded-time for the `.Bd()` rate extension.
use embedded_time::rate::Extensions;
use stm32l0xx_hal::{pac, prelude::*, rcc::Config, serial};

#[entry]
fn main() -> ! {
    let dp = pac::Peripherals::take().unwrap();

    let mut rcc = dp.RCC.freeze(Config::hsi16());
    let gpioa = dp.GPIOA.split(&mut rcc);

    let serial = dp
        .USART2
        .usart(
            gpioa.pa2,
            gpioa.pa3,
            serial::Config::default().baudrate(115_200_u32.Bd()),
            &mut rcc,
        )
        .unwrap();

    let (mut tx, _) = serial.split();

    let msg = b"Hello world!\n";
    for &byte in msg {
        let _ = block!(tx.write(byte));
    }

    loop {}
}
