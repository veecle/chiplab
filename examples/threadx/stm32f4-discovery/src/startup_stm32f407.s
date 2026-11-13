/* Minimal STM32F407 startup: vector table, reset handler (VTOR, FPU enable, .data
 * copy, .bss zero), and a default handler. PendSV binds to the ThreadX port;
 * SysTick binds to this example's handler in tx_low_level.c. */

    .syntax unified
    .cpu cortex-m4
    .fpu fpv4-sp-d16
    .thumb

    .global _vectors
    .global Reset_Handler

    .section .isr_vector,"a",%progbits
    .type _vectors, %object
_vectors:
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
    .word Default_Handler   /* SVCall   — unused by the Cortex-M ThreadX port */
    .word Default_Handler   /* DebugMon */
    .word 0
    .word PendSV_Handler    /* PendSV   — ThreadX port context switch */
    .word SysTick_Handler   /* SysTick  — ThreadX kernel tick */
    .size _vectors, . - _vectors

    .section .text.Reset_Handler
    .weak Reset_Handler
    .type Reset_Handler, %function
Reset_Handler:
    ldr   sp, =_estack

    /* Point VTOR at the vector table in FLASH rather than relying on the boot alias at 0. */
    ldr   r0, =0xE000ED08
    ldr   r1, =_vectors
    str   r1, [r0]

    /* Grant full access to CP10/CP11 — the firmware is built for the hard-float
     * ABI, so the FPU must be on before any VFP instruction retires. */
    ldr   r0, =0xE000ED88
    ldr   r1, [r0]
    orr   r1, r1, #(0xF << 20)
    str   r1, [r0]
    dsb
    isb

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
