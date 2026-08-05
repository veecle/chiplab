/* STM32F103 Blue Pill: a FreeRTOS task prints "Hello world!" over USART2 (PA2/PA3). */

#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"

#define PERIPH_BASE        0x40000000UL
#define APB1_BASE          ( PERIPH_BASE + 0x00000000UL )
#define APB2_BASE          ( PERIPH_BASE + 0x00010000UL )

#define RCC_BASE           ( PERIPH_BASE + 0x00021000UL )
#define RCC_APB2ENR        ( *( volatile uint32_t * )( RCC_BASE + 0x18UL ) )
#define RCC_APB1ENR        ( *( volatile uint32_t * )( RCC_BASE + 0x1CUL ) )
#define RCC_APB2ENR_IOPAEN ( 1UL << 2 )
#define RCC_APB1ENR_USART2EN ( 1UL << 17 )

#define GPIOA_BASE         ( APB2_BASE + 0x0800UL )
#define GPIOA_CRL          ( *( volatile uint32_t * )( GPIOA_BASE + 0x00UL ) )

#define USART2_BASE        ( APB1_BASE + 0x4400UL )
#define USART2_SR          ( *( volatile uint32_t * )( USART2_BASE + 0x00UL ) )
#define USART2_DR          ( *( volatile uint32_t * )( USART2_BASE + 0x04UL ) )
#define USART2_BRR         ( *( volatile uint32_t * )( USART2_BASE + 0x08UL ) )
#define USART2_CR1         ( *( volatile uint32_t * )( USART2_BASE + 0x0CUL ) )
#define USART_SR_TXE       ( 1UL << 7 )
#define USART_SR_TC        ( 1UL << 6 )
#define USART_CR1_UE       ( 1UL << 13 )
#define USART_CR1_TE       ( 1UL << 3 )

static void uart_init( void )
{
    RCC_APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC_APB1ENR |= RCC_APB1ENR_USART2EN;

    /* PA2 (TX) to AF push-pull output (CNF=10, MODE=10), PA3 (RX) to floating
     * input (CNF=01, MODE=00), in the legacy F1 CRL 4-bit-per-pin scheme. */
    GPIOA_CRL &= ~( ( 0xFUL << ( 4 * 2 ) ) | ( 0xFUL << ( 4 * 3 ) ) );
    GPIOA_CRL |= ( 0xAUL << ( 4 * 2 ) ) | ( 0x4UL << ( 4 * 3 ) );

    /* PCLK1 = 8 MHz (HSI reset default, no PLL, no prescaler);
     * BRR = fck / baud = 8e6 / 115200 = 69. */
    USART2_BRR = 69;
    USART2_CR1 = USART_CR1_UE | USART_CR1_TE;
}

static void uart_write( const char *s )
{
    while( *s )
    {
        while( !( USART2_SR & USART_SR_TXE ) )
        {
        }
        USART2_DR = ( uint32_t )( uint8_t )*s++;
    }
    /* Block until the last byte has fully shifted out, so the simulation window
     * doesn't close with bytes still sitting in the TX register. */
    while( !( USART2_SR & USART_SR_TC ) )
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
