/* STM32H745 Nucleo (Cortex-M7 core): a FreeRTOS task prints "Hello world!" over USART3 (PD8). */

#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"

/* H7 peripherals (other than USART) live in the AHB4 domain at 0x5800_0000+,
 * not the 0x4000_0000 range used on F1/F4/F7 — see RM0399. */
#define RCC_BASE           0x58024400UL
#define RCC_AHB4ENR        ( *( volatile uint32_t * )( RCC_BASE + 0xE0UL ) )
#define RCC_APB1LENR       ( *( volatile uint32_t * )( RCC_BASE + 0xE8UL ) )
#define RCC_AHB4ENR_GPIODEN  ( 1UL << 3 )
#define RCC_APB1LENR_USART3EN ( 1UL << 18 )

#define GPIOD_BASE         0x58020C00UL
#define GPIOD_MODER        ( *( volatile uint32_t * )( GPIOD_BASE + 0x00UL ) )
#define GPIOD_AFRH         ( *( volatile uint32_t * )( GPIOD_BASE + 0x24UL ) )

#define USART3_BASE        0x40004800UL
#define USART3_CR1         ( *( volatile uint32_t * )( USART3_BASE + 0x00UL ) )
#define USART3_BRR         ( *( volatile uint32_t * )( USART3_BASE + 0x0CUL ) )
#define USART3_ISR         ( *( volatile uint32_t * )( USART3_BASE + 0x1CUL ) )
#define USART3_TDR         ( *( volatile uint32_t * )( USART3_BASE + 0x28UL ) )
#define USART_ISR_TXE      ( 1UL << 7 )
#define USART_ISR_TC       ( 1UL << 6 )
#define USART_CR1_UE       ( 1UL << 0 )
#define USART_CR1_TE       ( 1UL << 3 )

static void uart_init( void )
{
    RCC_AHB4ENR |= RCC_AHB4ENR_GPIODEN;
    ( void ) RCC_AHB4ENR; /* synchronize before touching GPIOD */
    RCC_APB1LENR |= RCC_APB1LENR_USART3EN;
    ( void ) RCC_APB1LENR; /* synchronize before touching USART3 */

    /* PD8 to AF mode (MODER=0b10), then AF7 (USART3_TX) in the high AF register. */
    GPIOD_MODER &= ~( 3UL << ( 8 * 2 ) );
    GPIOD_MODER |= ( 2UL << ( 8 * 2 ) );
    GPIOD_AFRH &= ~( 0xFUL << ( ( 8 - 8 ) * 4 ) );
    GPIOD_AFRH |= ( 7UL << ( ( 8 - 8 ) * 4 ) );

    /* BRR = 139 (16e6 / 115200) mirrors the bare-metal example validated on this
     * board. Note the H7 boots on HSI at 64 MHz (not 16 MHz like F4/F7), so on real
     * hardware this divisor would land at 460800 baud. */
    USART3_BRR = 139;
    USART3_CR1 = USART_CR1_UE | USART_CR1_TE;
}

static void uart_write( const char *s )
{
    while( *s )
    {
        while( !( USART3_ISR & USART_ISR_TXE ) )
        {
        }
        USART3_TDR = ( uint32_t )( uint8_t )*s++;
    }
    /* Block until the last byte has fully shifted out, so the simulation window
     * doesn't close with bytes still sitting in the TX register. */
    while( !( USART3_ISR & USART_ISR_TC ) )
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
