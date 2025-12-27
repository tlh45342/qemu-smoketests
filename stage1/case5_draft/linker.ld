ENTRY(_start)

MEMORY
{
  RAM (rwx) : ORIGIN = 0x00000000, LENGTH = 128M
}

SECTIONS
{
  . = ORIGIN(RAM);

  .text : ALIGN(4)
  {
    KEEP(*(.vectors))
    *(.text*)
    *(.rodata*)
  } > RAM

  .data : ALIGN(4)
  {
    *(.data*)
  } > RAM

  .bss : ALIGN(8)
  {
    __bss_start__ = .;
    *(.bss*)
    *(COMMON)
    __bss_end__ = .;
  } > RAM

  /* Simple fixed stacks near the top of RAM */
  __stack_top__     = ORIGIN(RAM) + LENGTH(RAM);

  __irq_stack_top__ = __stack_top__;
  __svc_stack_top__ = __irq_stack_top__ - 0x1000; /* 4KB IRQ stack */
}