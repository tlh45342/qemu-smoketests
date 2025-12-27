/* case4/linker.ld */
ENTRY(_start)

MEMORY
{
  RAM (rwx) : ORIGIN = 0x00000000, LENGTH = 128M
}

SECTIONS
{
  . = 0x00008000;

  .text : {
    *(.text.start*)
    *(.text*)
    *(.rodata*)
  } > RAM

  .data : {
    *(.data*)
  } > RAM

  .bss (NOLOAD) : {
    __bss_start__ = .;
    *(.bss*)
    *(COMMON)
    __bss_end__ = .;
  } > RAM

  /* Provide a simple stack top */
  __stack_top__ = ORIGIN(RAM) + LENGTH(RAM) - 0x1000;
}