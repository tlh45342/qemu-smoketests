



This is code that I am using to test QEMU with arm.

This will be my reference code.  The theory should be simple.  I get it working in QEMU.
Then I test the same code in my ARM-VM to validate that.

This is based on the ideal that KERNEL development is done created on a series of stages.
Each stage is proof; or validates; both code and hardware.

This code is stage 1.

The code is set to be simple.  To build all you should have to do is to utilize the Makefile.  Where possible I have set it to not use "linder.ld"
To test the code one should get by with using "make run"  that should engage qemu and with the parameters of the machine that we have set

Case 1: simple UART test

Case 2: Simple Framebuffer test (draw a box)

Case 3: Use frame buffer; draw box again and put simple characters.