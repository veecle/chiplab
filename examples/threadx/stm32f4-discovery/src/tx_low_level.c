/* STM32F407 low-level hooks the ThreadX Cortex-M port expects the application to
 * supply: kernel timer setup plus the SysTick handler that drives it. The port ships
 * a reference implementation in ports/cortex_m4/gnu/example_build; this is the same
 * contract written in C against this board's 16 MHz HSI clock. */

#include <stdint.h>

#define SCB_VTOR              ( *( volatile uint32_t * )0xE000ED08UL )
#define SCB_SHPR2             ( *( volatile uint32_t * )0xE000ED1CUL )
#define SCB_SHPR3             ( *( volatile uint32_t * )0xE000ED20UL )

#define SYSTICK_CTRL          ( *( volatile uint32_t * )0xE000E010UL )
#define SYSTICK_LOAD          ( *( volatile uint32_t * )0xE000E014UL )
#define SYSTICK_CTRL_ENABLE   ( 1UL << 0 )
#define SYSTICK_CTRL_TICKINT  ( 1UL << 1 )
#define SYSTICK_CTRL_CLKSOURCE ( 1UL << 2 )

#define SYSTEM_CLOCK_HZ       16000000UL
#define TICKS_PER_SECOND      100UL

/* Defined by the linker script and the startup file. */
extern uint32_t _estack;
extern uint32_t _ebss;
extern const uint32_t _vectors[];

/* Kernel-owned globals and the port's tick entry point. */
extern void *_tx_thread_system_stack_ptr;
extern void *_tx_initialize_unused_memory;
void _tx_timer_interrupt( void );

void _tx_initialize_low_level( void )
{
    /* Interrupts stay masked until the scheduler starts the first thread. */
    __asm volatile ( "cpsid i" ::: "memory" );

    _tx_initialize_unused_memory = ( void * ) &_ebss;
    _tx_thread_system_stack_ptr = ( void * ) &_estack;

    SCB_VTOR = ( uint32_t ) _vectors;

    /* SVCall and PendSV must sit at the lowest priority (0xFF) for the port's
     * context switch to be tail-chained after every other exception. */
    SCB_SHPR2 = 0xFF000000UL;
    SCB_SHPR3 = 0x40FF0000UL;

    SYSTICK_LOAD = ( SYSTEM_CLOCK_HZ / TICKS_PER_SECOND ) - 1UL;
    SYSTICK_CTRL = SYSTICK_CTRL_CLKSOURCE | SYSTICK_CTRL_TICKINT | SYSTICK_CTRL_ENABLE;
}

void SysTick_Handler( void )
{
    _tx_timer_interrupt();
}
