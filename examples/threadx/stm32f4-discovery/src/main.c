/* STM32F4 Discovery: a ThreadX thread prints "Hello world!" over USART2 (PA2/PA3). */

#include <stdint.h>

#include "tx_api.h"

#define PERIPH_BASE          0x40000000UL
#define AHB1_BASE            ( PERIPH_BASE + 0x00020000UL )
#define APB1_BASE            ( PERIPH_BASE + 0x00000000UL )

#define RCC_BASE             ( AHB1_BASE + 0x3800UL )
#define RCC_AHB1ENR          ( *( volatile uint32_t * )( RCC_BASE + 0x30UL ) )
#define RCC_APB1ENR          ( *( volatile uint32_t * )( RCC_BASE + 0x40UL ) )
#define RCC_AHB1ENR_GPIOAEN  ( 1UL << 0 )
#define RCC_APB1ENR_USART2EN ( 1UL << 17 )

#define GPIOA_BASE           ( AHB1_BASE + 0x0000UL )
#define GPIOA_MODER          ( *( volatile uint32_t * )( GPIOA_BASE + 0x00UL ) )
#define GPIOA_AFRL           ( *( volatile uint32_t * )( GPIOA_BASE + 0x20UL ) )

#define USART2_BASE          ( APB1_BASE + 0x4400UL )
#define USART2_SR            ( *( volatile uint32_t * )( USART2_BASE + 0x00UL ) )
#define USART2_DR            ( *( volatile uint32_t * )( USART2_BASE + 0x04UL ) )
#define USART2_BRR           ( *( volatile uint32_t * )( USART2_BASE + 0x08UL ) )
#define USART2_CR1           ( *( volatile uint32_t * )( USART2_BASE + 0x0CUL ) )
#define USART_SR_TXE         ( 1UL << 7 )
#define USART_SR_TC          ( 1UL << 6 )
#define USART_CR1_UE         ( 1UL << 13 )
#define USART_CR1_TE         ( 1UL << 3 )

#define HELLO_STACK_WORDS    256
#define HELLO_PRIORITY       1

static TX_THREAD hello_thread;
static ULONG hello_stack[ HELLO_STACK_WORDS ];

static void uart_init( void )
{
    RCC_AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC_APB1ENR |= RCC_APB1ENR_USART2EN;

    /* PA2/PA3 to AF mode (MODER=0b10), then AF7 (USART2) in the low AF register. */
    GPIOA_MODER &= ~( ( 3UL << ( 2 * 2 ) ) | ( 3UL << ( 3 * 2 ) ) );
    GPIOA_MODER |= ( 2UL << ( 2 * 2 ) ) | ( 2UL << ( 3 * 2 ) );
    GPIOA_AFRL &= ~( ( 0xFUL << ( 2 * 4 ) ) | ( 0xFUL << ( 3 * 4 ) ) );
    GPIOA_AFRL |= ( 7UL << ( 2 * 4 ) ) | ( 7UL << ( 3 * 4 ) );

    /* PCLK1 = 16 MHz (HSI, no prescaler); BRR = fck / baud = 16e6 / 115200 = 139. */
    USART2_BRR = 139;
    USART2_CR1 = USART_CR1_UE | USART_CR1_TE;
}

static void uart_write( const char *s )
{
    while( *s )
    {
        while( !( USART2_SR & USART_SR_TXE ) )
        {
        }
        USART2_DR = ( uint32_t )( uint8_t ) * s++;
    }
    /* Block until the last byte has fully shifted out, so the simulation window
     * doesn't close with bytes still sitting in the TX register. */
    while( !( USART2_SR & USART_SR_TC ) )
    {
    }
}

static void hello_entry( ULONG input )
{
    ( void ) input;
    for( ;; )
    {
        uart_write( "Hello world!\n" );
        tx_thread_sleep( TX_TIMER_TICKS_PER_SECOND );
    }
}

/* ThreadX calls this from tx_kernel_enter() to create the application's objects.
 * Stacks are static arrays, so the first-unused-memory pointer goes unused. */
void tx_application_define( void *first_unused_memory )
{
    ( void ) first_unused_memory;

    tx_thread_create( &hello_thread, "hello", hello_entry, 0,
                      hello_stack, sizeof( hello_stack ),
                      HELLO_PRIORITY, HELLO_PRIORITY,
                      TX_NO_TIME_SLICE, TX_AUTO_START );
}

int main( void )
{
    uart_init();

    tx_kernel_enter();

    for( ;; )
    {
    }
}
