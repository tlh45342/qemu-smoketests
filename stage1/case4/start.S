/* case4/start.S */
    .syntax unified
    .cpu arm926ej-s
    .arm

    .global _start
    .extern main
    .extern __bss_start__
    .extern __bss_end__
    .extern __stack_top__

_start:
    /* Enter SVC mode, disable IRQ/FIQ */
    mrs r0, cpsr
    bic r0, r0, #0x1F
    orr r0, r0, #0x13
    orr r0, r0, #(1<<7)     /* I */
    orr r0, r0, #(1<<6)     /* F */
    msr cpsr_c, r0

    /* Set stack */
    ldr sp, =__stack_top__

    /* Zero BSS */
    ldr r1, =__bss_start__
    ldr r2, =__bss_end__
    mov r0, #0
1:
    cmp r1, r2
    bhs 2f
    str r0, [r1], #4
    b 1b
2:
    bl main

hang:
    b hang