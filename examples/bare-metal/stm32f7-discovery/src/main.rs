//! Hello world over USART2 (PA2/PA3) on the STM32F7 Discovery.

#![no_main]
#![no_std]

use cortex_m_rt::entry;
use nb::block;
use panic_halt as _;
use stm32f7xx_hal::{
    pac,
    prelude::*,
    serial::{Config, Serial},
};

#[entry]
fn main() -> ! {
    let dp = pac::Peripherals::take().unwrap();

    let rcc = dp.RCC.constrain();
    let clocks = rcc.cfgr.sysclk(16.MHz()).freeze();

    let gpioa = dp.GPIOA.split();
    let tx = gpioa.pa2.into_alternate::<7>();
    let rx = gpioa.pa3.into_alternate::<7>();

    let serial = Serial::new(dp.USART2, (tx, rx), &clocks, Config::default());

    let (mut tx, _) = serial.split();

    let msg = b"Hello world!\n";
    for &byte in msg {
        let _ = block!(tx.write(byte));
    }

    loop {}
}
