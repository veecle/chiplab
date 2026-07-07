//! Hello world over USART2 (PA2/PA3) on the STM32F103 Blue Pill.

#![no_main]
#![no_std]

use cortex_m_rt::entry;
use nb::block;
use panic_halt as _;
use stm32f1xx_hal::{pac, prelude::*, serial::Config};

#[entry]
fn main() -> ! {
    let dp = pac::Peripherals::take().unwrap();

    let mut rcc = dp.RCC.constrain();

    let mut gpioa = dp.GPIOA.split(&mut rcc);
    let tx = gpioa.pa2.into_alternate_push_pull(&mut gpioa.crl);
    let rx = gpioa.pa3;

    let serial = dp.USART2.serial(
        (tx, rx),
        Config::default().baudrate(115_200.bps()),
        &mut rcc,
    );

    let (mut tx, _) = serial.split();

    let msg = b"Hello world!\n";
    for &byte in msg {
        let _ = block!(tx.write(byte));
    }

    loop {}
}
