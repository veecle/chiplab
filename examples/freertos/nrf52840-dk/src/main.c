/* nRF52840 DK: a FreeRTOS task prints "Hello world!" over UARTE0 (P0.06 TX / P0.08 RX). */

#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"

#define GPIO_P0_BASE       0x50000000UL
#define GPIO_OUTSET        ( *( volatile uint32_t * )( GPIO_P0_BASE + 0x508UL ) )
#define GPIO_DIRSET        ( *( volatile uint32_t * )( GPIO_P0_BASE + 0x518UL ) )

#define TXD_PIN            6U
#define PSEL_DISCONNECTED  0xFFFFFFFFUL

#define UARTE0_BASE            0x40002000UL
#define UARTE0_TASKS_STARTTX   ( *( volatile uint32_t * )( UARTE0_BASE + 0x008UL ) )
#define UARTE0_TASKS_STOPTX    ( *( volatile uint32_t * )( UARTE0_BASE + 0x00CUL ) )
#define UARTE0_EVENTS_ENDTX    ( *( volatile uint32_t * )( UARTE0_BASE + 0x120UL ) )
#define UARTE0_ENABLE          ( *( volatile uint32_t * )( UARTE0_BASE + 0x500UL ) )
#define UARTE0_PSEL_TXD        ( *( volatile uint32_t * )( UARTE0_BASE + 0x50CUL ) )
#define UARTE0_PSEL_RXD        ( *( volatile uint32_t * )( UARTE0_BASE + 0x514UL ) )
#define UARTE0_BAUDRATE        ( *( volatile uint32_t * )( UARTE0_BASE + 0x524UL ) )
#define UARTE0_TXD_PTR         ( *( volatile uint32_t * )( UARTE0_BASE + 0x544UL ) )
#define UARTE0_TXD_MAXCNT      ( *( volatile uint32_t * )( UARTE0_BASE + 0x548UL ) )

#define UARTE_ENABLE_ENABLED   8UL
#define UARTE_BAUDRATE_115200  0x01D60000UL

static void uart_init( void )
{
    /* P0.06 must be driven as a GPIO output before UARTE claims it. */
    GPIO_OUTSET = ( 1UL << TXD_PIN );
    GPIO_DIRSET = ( 1UL << TXD_PIN );

    /* PSEL must be configured before ENABLE. RXD is unused, so disconnect it. */
    UARTE0_PSEL_TXD = TXD_PIN;
    UARTE0_PSEL_RXD = PSEL_DISCONNECTED;

    UARTE0_ENABLE = UARTE_ENABLE_ENABLED;
    UARTE0_BAUDRATE = UARTE_BAUDRATE_115200;
}

static void uart_write( const char *s, uint32_t len )
{
    /* EasyDMA requires the source buffer to live in RAM; the caller passes a
     * RAM-resident buffer (never a string literal, which lives in flash). */
    UARTE0_TXD_PTR = ( uint32_t )s;
    UARTE0_TXD_MAXCNT = len;
    UARTE0_EVENTS_ENDTX = 0;
    UARTE0_TASKS_STARTTX = 1;

    while( !UARTE0_EVENTS_ENDTX )
    {
    }
    UARTE0_EVENTS_ENDTX = 0;
    UARTE0_TASKS_STOPTX = 1;
}

static void hello_task( void *pvParameters )
{
    ( void )pvParameters;
    /* static (not const) so the buffer lives in RAM — EasyDMA can't read flash. */
    static char message[] = "Hello world!\n";

    for( ;; )
    {
        uart_write( message, sizeof( message ) - 1 );
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
