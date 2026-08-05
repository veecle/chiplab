/* STM32L073 Nucleo: a FreeRTOS task prints "Hello world!" over USART2 (PA2/PA3). */

#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"

#define RCC_BASE           0x40021000UL
#define RCC_CR             ( *( volatile uint32_t * )( RCC_BASE + 0x00UL ) )
#define RCC_CFGR           ( *( volatile uint32_t * )( RCC_BASE + 0x0CUL ) )
#define RCC_IOPENR         ( *( volatile uint32_t * )( RCC_BASE + 0x2CUL ) )
#define RCC_APB1ENR        ( *( volatile uint32_t * )( RCC_BASE + 0x38UL ) )
#define RCC_CR_HSI16ON     ( 1UL << 0 )
#define RCC_CR_HSI16RDY    ( 1UL << 2 )
#define RCC_CFGR_SW_HSI16  ( 1UL << 0 )
#define RCC_CFGR_SW_MASK   ( 3UL << 0 )
#define RCC_IOPENR_IOPAEN  ( 1UL << 0 )
#define RCC_APB1ENR_USART2EN ( 1UL << 17 )

#define GPIOA_BASE         0x50000000UL
#define GPIOA_MODER        ( *( volatile uint32_t * )( GPIOA_BASE + 0x00UL ) )
#define GPIOA_AFRL         ( *( volatile uint32_t * )( GPIOA_BASE + 0x20UL ) )

/* STM32L0's USART2 uses the newer ISR/TDR register map (not the classic F1/F4
 * SR/DR), and CR1.UE/TE sit at bits 0/3 here, not F4's bit 13/3. */
#define USART2_BASE        0x40004400UL
#define USART2_CR1         ( *( volatile uint32_t * )( USART2_BASE + 0x00UL ) )
#define USART2_BRR         ( *( volatile uint32_t * )( USART2_BASE + 0x0CUL ) )
#define USART2_ISR         ( *( volatile uint32_t * )( USART2_BASE + 0x1CUL ) )
#define USART2_TDR         ( *( volatile uint32_t * )( USART2_BASE + 0x28UL ) )
#define USART_ISR_TXE      ( 1UL << 7 )
#define USART_ISR_TC       ( 1UL << 6 )
#define USART_CR1_UE       ( 1UL << 0 )
#define USART_CR1_TE       ( 1UL << 3 )

static void clock_init( void )
{
    /* L0 boots on MSI (~2.1 MHz); switch SYSCLK to HSI16 so the USART BRR
     * divisor below (computed for 16 MHz) is valid. */
    RCC_CR |= RCC_CR_HSI16ON;
    while( !( RCC_CR & RCC_CR_HSI16RDY ) )
    {
    }

    /* Chiplab's simulated RCC reflects HSI16RDY but not SWS after the SW
     * write below, so the switch is fire-and-forget here rather than polled
     * to completion (polling SWS would spin forever in simulation). */
    RCC_CFGR = ( RCC_CFGR & ~RCC_CFGR_SW_MASK ) | RCC_CFGR_SW_HSI16;
}

static void uart_init( void )
{
    RCC_IOPENR |= RCC_IOPENR_IOPAEN;
    ( void )RCC_IOPENR;   /* read-back fence before touching GPIOA */
    RCC_APB1ENR |= RCC_APB1ENR_USART2EN;
    ( void )RCC_APB1ENR;  /* read-back fence before touching USART2 */

    /* PA2/PA3 to AF mode (MODER=0b10), then AF4 (USART2) in the low AF register. */
    GPIOA_MODER &= ~( ( 3UL << ( 2 * 2 ) ) | ( 3UL << ( 3 * 2 ) ) );
    GPIOA_MODER |= ( 2UL << ( 2 * 2 ) ) | ( 2UL << ( 3 * 2 ) );
    GPIOA_AFRL &= ~( ( 0xFUL << ( 2 * 4 ) ) | ( 0xFUL << ( 3 * 4 ) ) );
    GPIOA_AFRL |= ( 4UL << ( 2 * 4 ) ) | ( 4UL << ( 3 * 4 ) );

    /* fCK = 16 MHz (HSI16, no prescaler); BRR = fck / baud = 16e6 / 115200 = 139. */
    USART2_BRR = 139;
    USART2_CR1 = USART_CR1_UE | USART_CR1_TE;
}

static void uart_write( const char *s )
{
    while( *s )
    {
        while( !( USART2_ISR & USART_ISR_TXE ) )
        {
        }
        USART2_TDR = ( uint32_t )( uint8_t )*s++;
    }
    /* Block until the last byte has fully shifted out, so the simulation window
     * doesn't close with bytes still sitting in the TX register. */
    while( !( USART2_ISR & USART_ISR_TC ) )
    {
    }
}

static void hello_task( void *pvParameters )
{
    ( void )pvParameters;
    for( ;; )
    {
        uart_write( "Hello world!\n" );
        vTaskDelay( pdMS_TO_TICKS( 1000 ) );
    }
}

int main( void )
{
    clock_init();
    uart_init();

    xTaskCreate( hello_task, "hello", configMINIMAL_STACK_SIZE, NULL,
                 configMAX_PRIORITIES - 1, NULL );

    vTaskStartScheduler();

    for( ;; )
    {
    }
}

void vApplicationMallocFailedHook( void )
{
    taskDISABLE_INTERRUPTS();
    for( ;; )
    {
    }
}
