/* Minimal STM32F407 startup: vector table, reset handler (.data copy + .bss zero),
 * and weak default handlers. The three kernel-owned exceptions (SVC/PendSV/SysTick)
 * are left non-weak so they bind to the FreeRTOS port implementations. */

    .syntax unified
    .cpu cortex-m4
    .fpu softvfp
    .thumb

    .global g_pfnVectors
    .global Reset_Handler

    .word _sidata          /* .data load address in FLASH */
    .word _sdata           /* .data start in RAM */
    .word _edata           /* .data end in RAM */
    .word _sbss            /* .bss start in RAM */
    .word _ebss            /* .bss end in RAM */

    .section .isr_vector,"a",%progbits
    .type g_pfnVectors, %object
g_pfnVectors:
    .word _estack
    .word Reset_Handler
    .word Default_Handler   /* NMI */
    .word Default_Handler   /* HardFault */
    .word Default_Handler   /* MemManage */
    .word Default_Handler   /* BusFault */
    .word Default_Handler   /* UsageFault */
    .word 0
    .word 0
    .word 0
    .word 0
    .word SVC_Handler       /* SVCall   — FreeRTOS port */
    .word Default_Handler   /* DebugMon */
    .word 0
    .word PendSV_Handler    /* PendSV   — FreeRTOS port */
    .word SysTick_Handler   /* SysTick  — FreeRTOS port */
    .size g_pfnVectors, . - g_pfnVectors

    .section .text.Reset_Handler
    .weak Reset_Handler
    .type Reset_Handler, %function
Reset_Handler:
    ldr   sp, =_estack

    /* Copy .data from its FLASH load address into RAM. */
    ldr   r0, =_sdata
    ldr   r1, =_edata
    ldr   r2, =_sidata
    movs  r3, #0
    b     LoopCopyData
CopyData:
    ldr   r4, [r2, r3]
    str   r4, [r0, r3]
    adds  r3, r3, #4
LoopCopyData:
    adds  r4, r0, r3
    cmp   r4, r1
    bcc   CopyData

    /* Zero .bss. */
    ldr   r2, =_sbss
    ldr   r4, =_ebss
    movs  r3, #0
    b     LoopZeroBss
ZeroBss:
    str   r3, [r2]
    adds  r2, r2, #4
LoopZeroBss:
    cmp   r2, r4
    bcc   ZeroBss

    bl    main
LoopForever:
    b     LoopForever
    .size Reset_Handler, . - Reset_Handler

    .section .text.Default_Handler,"ax",%progbits
    .type Default_Handler, %function
Default_Handler:
    b     Default_Handler
    .size Default_Handler, . - Default_Handler
