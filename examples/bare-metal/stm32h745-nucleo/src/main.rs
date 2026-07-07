//! Hello world on the STM32H745 Nucleo via direct USART3 register writes.

#![no_main]
#![no_std]

use cortex_m_rt::entry;
use panic_halt as _;
// HAL pulled in only for its interrupt vector table.
use stm32h7xx_hal as _;

const USART3_BASE: u32 = 0x4000_4800;
const USART3_CR1: *mut u32 = (USART3_BASE + 0x00) as *mut u32;
const USART3_BRR: *mut u32 = (USART3_BASE + 0x0C) as *mut u32;
const USART3_ISR: *const u32 = (USART3_BASE + 0x1C) as *const u32;
const USART3_TDR: *mut u32 = (USART3_BASE + 0x28) as *mut u32;

#[entry]
fn main() -> ! {
    unsafe {
        USART3_BRR.write_volatile(0x8B);
        USART3_CR1.write_volatile(0b1001); // TE | UE

        for &byte in b"Hello world!\n" {
            while USART3_ISR.read_volatile() & (1 << 7) == 0 {} // wait for TXE
            USART3_TDR.write_volatile(byte as u32);
        }
    }

    loop {}
}
