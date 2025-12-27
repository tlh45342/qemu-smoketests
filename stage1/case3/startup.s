  .section .text
    .global _start

_start:
    // -m 128M => RAM ends at 0x08000000, stack grows down
    ldr   sp, =0x08000000

    bl    main

1:  b     1b       // If main returns, loop forever