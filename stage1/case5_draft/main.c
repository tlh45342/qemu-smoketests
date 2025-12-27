#include <stdint.h>
#include <stddef.h>

/* --- VersatilePB UART0 (PL011) --- */
#define UART0_BASE   0x101F1000u
#define UARTDR       (*(volatile uint32_t *)(UART0_BASE + 0x00))
#define UARTFR       (*(volatile uint32_t *)(UART0_BASE + 0x18))
#define UARTIBRD     (*(volatile uint32_t *)(UART0_BASE + 0x24))
#define UARTFBRD     (*(volatile uint32_t *)(UART0_BASE + 0x28))
#define UARTLCRH     (*(volatile uint32_t *)(UART0_BASE + 0x2C))
#define UARTCR       (*(volatile uint32_t *)(UART0_BASE + 0x30))

/* UARTFR bits */
#define UARTFR_TXFF  (1u << 5)

/* --- PL190 VIC (VersatilePB) --- */
#define VIC_BASE       0x10140000u
#define VICIntSelect   (*(volatile uint32_t *)(VIC_BASE + 0x0C))
#define VICIntEnable   (*(volatile uint32_t *)(VIC_BASE + 0x10))
#define VICVectAddr    (*(volatile uint32_t *)(VIC_BASE + 0x30))

/* Timer0/1 share IRQ line 4 on VersatilePB */
#define VIC_IRQ_TIMER01 4u

/* --- SP804 Dual Timer (Timer0/1 block) --- */
#define TIMER01_BASE   0x101E2000u

/* Use Timer1 registers at base + 0x00.. */
#define T1_LOAD        (*(volatile uint32_t *)(TIMER01_BASE + 0x00))
#define T1_VALUE       (*(volatile uint32_t *)(TIMER01_BASE + 0x04))
#define T1_CONTROL     (*(volatile uint32_t *)(TIMER01_BASE + 0x08))
#define T1_INTCLR      (*(volatile uint32_t *)(TIMER01_BASE + 0x0C))
#define T1_MIS         (*(volatile uint32_t *)(TIMER01_BASE + 0x14))
#define T1_BGLOAD      (*(volatile uint32_t *)(TIMER01_BASE + 0x18))

/* Control bits (SP804) */
#define TCTRL_ENABLE      (1u << 7)
#define TCTRL_PERIODIC    (1u << 6)
#define TCTRL_INT_ENABLE  (1u << 5)
#define TCTRL_32BIT       (1u << 1)

/* Linker symbols for BSS clearing */
extern uint32_t __bss_start__;
extern uint32_t __bss_end__;

static void uart_init(void) {
    /* Disable UART while configuring */
    UARTCR = 0;

    /* Typical VersatilePB UART clock is treated as 24MHz in many examples.
       Even if divisors are off, QEMU console will still show output fine. */
    UARTIBRD = 13; /* 24MHz/(16*115200) ≈ 13.02 */
    UARTFBRD = 2;

    /* 8N1 + FIFO enable */
    UARTLCRH = (3u << 5) | (1u << 4);

    /* Enable UART, TX, RX */
    UARTCR = (1u << 0) | (1u << 8) | (1u << 9);
}

static void uart_putc(char c) {
    while (UARTFR & UARTFR_TXFF) {
        /* wait */
    }
    UARTDR = (uint32_t)c;
}

static void enable_irqs(void) {
    /* ARM926: no CPSIE, so clear I bit in CPSR via MRS/MSR */
    uint32_t cpsr;
    __asm__ volatile ("mrs %0, cpsr" : "=r"(cpsr));
    cpsr &= ~0x80u; /* clear I */
    __asm__ volatile ("msr cpsr_c, %0" :: "r"(cpsr));
}

static void timer_init_5s_tick(void) {
    /* QEMU Versatile timer commonly uses 1MHz reference, so 5 seconds = 5,000,000 ticks */
    const uint32_t ticks = 5000000u;

    /* Disable timer while setting up */
    T1_CONTROL = 0;

    T1_LOAD   = ticks;
    T1_BGLOAD = ticks;

    /* Clear any pending interrupt */
    T1_INTCLR = 1;

    /* Periodic, 32-bit, interrupt enable, enable */
    T1_CONTROL = TCTRL_ENABLE | TCTRL_PERIODIC | TCTRL_INT_ENABLE | TCTRL_32BIT;
}

static void vic_init_for_timer(void) {
    /* Make sure this source is IRQ (not FIQ) */
    VICIntSelect &= ~(1u << VIC_IRQ_TIMER01);

    /* Enable timer IRQ line */
    VICIntEnable = (1u << VIC_IRQ_TIMER01);
}

/* Called from startup.S after stacks are set */
void c_startup(void) {
    /* Clear .bss */
    for (uint32_t *p = &__bss_start__; p < &__bss_end__; p++) {
        *p = 0;
    }

    /* Now call main */
    extern void main(void);
    main();

    for (;;) { /* never return */ }
}

static volatile uint32_t g_tick = 0;

/* This is invoked by the assembly IRQ wrapper */
void irq_handler_c(void) {
    /* Confirm timer asserted (MIS bit) */
    if (T1_MIS) {
        /* Clear timer interrupt (write-any) */
        T1_INTCLR = 1;

        /* Alternate output */
        char ch = (g_tick & 1u) ? 'B' : 'A';
        uart_putc(ch);

        g_tick++;
    }

    /* End of interrupt to VIC */
    VICVectAddr = 0;
}

void main(void) {
    uart_init();
    uart_putc('\n');
    uart_putc('S'); uart_putc('T'); uart_putc('A'); uart_putc('R'); uart_putc('T');
    uart_putc('\n');

    vic_init_for_timer();
    timer_init_5s_tick();

    enable_irqs();

    /* Idle forever; interrupts do the work */
    for (;;) {
        __asm__ volatile ("nop");
    }
}
