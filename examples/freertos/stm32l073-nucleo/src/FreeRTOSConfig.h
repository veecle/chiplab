/* FreeRTOS config for STM32L073 (Cortex-M0+) running on HSI16 at 16 MHz. */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#define configCPU_CLOCK_HZ                      ( 16000000UL )
#define configTICK_RATE_HZ                      ( 1000 )
#define configUSE_PREEMPTION                    1
#define configUSE_TIME_SLICING                  1
#define configMAX_PRIORITIES                    ( 5 )
#define configMINIMAL_STACK_SIZE                ( 128 )
#define configTOTAL_HEAP_SIZE                   ( 10 * 1024 )
#define configMAX_TASK_NAME_LEN                 ( 16 )
#define configUSE_16_BIT_TICKS                  0
#define configIDLE_SHOULD_YIELD                 1

#define configUSE_IDLE_HOOK                     0
#define configUSE_TICK_HOOK                     0
#define configUSE_MALLOC_FAILED_HOOK            1
#define configCHECK_FOR_STACK_OVERFLOW          0

#define configUSE_MUTEXES                       0
#define configUSE_RECURSIVE_MUTEXES             0
#define configUSE_COUNTING_SEMAPHORES           0
#define configUSE_QUEUE_SETS                    0
#define configUSE_TIMERS                        0
#define configUSE_CO_ROUTINES                   0
#define configUSE_TRACE_FACILITY                0
#define configGENERATE_RUN_TIME_STATS           0

#define INCLUDE_vTaskDelete                     0
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_vTaskSuspend                    0
#define INCLUDE_vTaskPrioritySet                0
#define INCLUDE_uxTaskPriorityGet               0

/* The ARM_CM0 port targets Cortex-M0/M0+, which has no BASEPRI register, so
 * (unlike the CM3/CM4 ports) there is no configPRIO_BITS / syscall-priority
 * shifting here: critical sections mask all interrupts via PRIMASK instead.
 * The port does require configENABLE_MPU to be defined (it #errors otherwise). */
#define configENABLE_MPU                        0

/* The CM0 port's SVC_Handler/PendSV_Handler/SysTick_Handler are already the
 * CMSIS vector-table names, so (unlike the CM3/CM4 ports) no #define mapping
 * is needed here — the startup file's vector table references them directly. */

#define configASSERT( x ) \
    if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); for( ;; ); }

#endif /* FREERTOS_CONFIG_H */
