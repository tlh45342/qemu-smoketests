// putchars_uart_c.c — QEMU VersatilePB (PL011 UART0) "Hello World"
// Run: qemu-system-arm -M versatilepb -m 128M -kernel firmware.bin -nographic -serial mon:stdio

#include <stdint.h>

// VersatilePB UART0 (PL011)
#define UART_BASE      0x101F1000u
#define UART_DR        (UART_BASE + 0x00)   // data
#define UART_FR        (UART_BASE + 0x18)   // flags
#define UART_FR_TXFF   (1u << 5)            // TX FIFO full

static inline void mmio_w32(uint32_t addr, uint32_t v) { *(volatile uint32_t*)addr = v; }
static inline uint32_t mmio_r32(uint32_t addr) { return *(volatile uint32_t*)addr; }

static void uart_putc(char c) {
    if (c == '\n') {
        while (mmio_r32(UART_FR) & UART_FR_TXFF) {}
        mmio_w32(UART_DR, (uint32_t)'\r');
    }
    while (mmio_r32(UART_FR) & UART_FR_TXFF) {}
    mmio_w32(UART_DR, (uint32_t)(uint8_t)c);
}

static void uart_write(const char *s) { while (*s) uart_putc(*s++); }

__attribute__((naked, noreturn))
void _start(void) {
    __asm__ volatile (
        // VersatilePB RAM is at 0x00000000; with -m 128M, top is 0x08000000
        "ldr sp, =0x08000000 \n"
        "bl  main            \n"
        "b   .               \n"
    );
}

int main(void) {
    uart_write("Hello World (QEMU versatilepb)!\n");
    for (;;) { /* spin */ }
}