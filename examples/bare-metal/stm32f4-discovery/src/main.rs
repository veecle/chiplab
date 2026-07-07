//! Hello world over USART2 (PA2/PA3) on the STM32F4 Discovery.

#![no_main]
#![no_std]

use cortex_m_rt::entry;
use panic_halt as _;
use stm32f4xx_hal::{pac, prelude::*, rcc::Config, serial::Serial};

#[entry]
fn main() -> ! {
    let dp = pac::Peripherals::take().unwrap();
    let mut rcc = dp.RCC.freeze(Config::hsi().sysclk(16.MHz()));
    let gpioa = dp.GPIOA.split(&mut rcc);

    let tx = gpioa.pa2;
    let rx = gpioa.pa3;

    let serial = Serial::new(
        dp.USART2,
        (tx, rx),
        stm32f4xx_hal::serial::Config::default().baudrate(115_200.bps()),
        &mut rcc,
    )
    .unwrap();

    let (mut tx, _) = serial.split();

    let msg = b"Hello world!\n";
    for &byte in msg {
        let _ = nb::block!(tx.write(byte));
    }

    loop {}
}
