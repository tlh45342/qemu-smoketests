.syntax unified
.cpu arm926ej-s
.arm

.equ MODE_IRQ, 0x12
.equ MODE_SVC, 0x13
.equ I_BIT,    0x80
.equ F_BIT,    0x40

.section .vectors, "ax"
.global _start
_start:
  b reset_handler          /* 0x00 Reset */
  b undef_handler          /* 0x04 Undefined */
  b swi_handler            /* 0x08 SWI */
  b pabort_handler         /* 0x0C Prefetch abort */
  b dabort_handler         /* 0x10 Data abort */
  b reserved_handler       /* 0x14 Reserved */
  b irq_handler            /* 0x18 IRQ */
  b fiq_handler            /* 0x1C FIQ */

.extern main
.extern c_irq_handler
.extern __bss_start__
.extern __bss_end__
.extern __svc_stack_top__
.extern __irq_stack_top__

reset_handler:
  /* Enter SVC mode, IRQ/FIQ disabled */
  mrs r0, cpsr
  bic r0, r0, #0x1F
  orr r0, r0, #MODE_SVC
  orr r0, r0, #(I_BIT|F_BIT)
  msr cpsr_c, r0

  /* Set SVC stack */
  ldr sp, =__svc_stack_top__

  /* Set IRQ stack */
  mrs r0, cpsr
  bic r0, r0, #0x1F
  orr r0, r0, #MODE_IRQ
  orr r0, r0, #(I_BIT|F_BIT)
  msr cpsr_c, r0
  ldr sp, =__irq_stack_top__

  /* Back to SVC */
  mrs r0, cpsr
  bic r0, r0, #0x1F
  orr r0, r0, #MODE_SVC
  orr r0, r0, #(I_BIT|F_BIT)
  msr cpsr_c, r0

  /* Clear .bss */
  ldr r1, =__bss_start__
  ldr r2, =__bss_end__
  mov r0, #0
bss_loop:
  cmp r1, r2
  bhs bss_done
  str r0, [r1], #4
  b bss_loop
bss_done:

  bl main
hang:
  b hang

irq_handler:
  /* simple IRQ wrapper -> C */
  sub lr, lr, #4
  stmfd sp!, {r0-r3, r12, lr}
  bl c_irq_handler
  ldmfd sp!, {r0-r3, r12, lr}
  subs pc, lr, #0

undef_handler:     b .
swi_handler:       b .
pabort_handler:    b .
dabort_handler:    b .
reserved_handler:  b .
fiq_handler:       b .