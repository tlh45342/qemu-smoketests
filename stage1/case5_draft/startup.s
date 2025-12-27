.syntax unified
.cpu arm926ej-s
.arm

/* ARM mode bits */
.equ MODE_USR, 0x10
.equ MODE_FIQ, 0x11
.equ MODE_IRQ, 0x12
.equ MODE_SVC, 0x13
.equ MODE_ABT, 0x17
.equ MODE_UND, 0x1B
.equ MODE_SYS, 0x1F

/* CPSR interrupt mask bits */
.equ I_BIT, 0x80
.equ F_BIT, 0x40

.global _start
.extern c_startup
.extern irq_handler_c

.section .vectors, "ax"
_start:
  b   reset_handler
  b   undef_handler
  b   swi_handler
  b   pabort_handler
  b   dabort_handler
  b   reserved_handler
  b   irq_vector
  b   fiq_handler

reset_handler:
  /* Start in SVC, IRQ/FIQ disabled */
  msr cpsr_c, #(MODE_SVC | I_BIT | F_BIT)

  /* Set SVC stack */
  ldr sp, =__svc_stack_top__

  /* Set IRQ stack */
  msr cpsr_c, #(MODE_IRQ | I_BIT | F_BIT)
  ldr sp, =__irq_stack_top__

  /* Back to SVC */
  msr cpsr_c, #(MODE_SVC | I_BIT | F_BIT)

  /* Jump into C startup (clears .bss, then main) */
  bl  c_startup

hang:
  b hang

irq_vector:
  /* Adjust LR_irq to point to interrupted instruction */
  sub lr, lr, #4

  /* Save minimal regs + lr */
  stmfd sp!, {r0-r3, r12, lr}

  /* Save SPSR */
  mrs r0, spsr
  stmfd sp!, {r0}

  bl  irq_handler_c

  /* Restore SPSR */
  ldmfd sp!, {r0}
  msr spsr_cxsf, r0

  /* Restore regs */
  ldmfd sp!, {r0-r3, r12, lr}

  /* Return from IRQ */
  subs pc, lr, #0

undef_handler:     b .
swi_handler:       b .
pabort_handler:    b .
dabort_handler:    b .
reserved_handler:  b .
fiq_handler:       b .

/* Linker-provided stack symbols */
.extern __svc_stack_top__
.extern __irq_stack_top__
