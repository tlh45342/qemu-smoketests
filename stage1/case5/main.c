#include <stdint.h>

/* UART0 (PL011) */
#define UART0_BASE   0x101F1000u
#define UARTDR       (*(volatile uint32_t *)(UART0_BASE + 0x00))
#define UARTFR       (*(volatile uint32_t *)(UART0_BASE + 0x18))
#define UARTIBRD     (*(volatile uint32_t *)(UART0_BASE + 0x24))
#define UARTFBRD     (*(volatile uint32_t *)(UART0_BASE + 0x28))
#define UARTLCRH     (*(volatile uint32_t *)(UART0_BASE + 0x2C))
#define UARTCR       (*(volatile uint32_t *)(UART0_BASE + 0x30))
#define UARTFR_TXFF  (1u << 5)

/* PL190 VIC (primary interrupt controller) */
#define VIC_BASE     0x10140000u
#define VICIRQSTATUS (*(volatile uint32_t *)(VIC_BASE + 0x00))
#define VICINTENABLE (*(volatile uint32_t *)(VIC_BASE + 0x10))
#define VICINTENCLEAR (*(volatile uint32_t *)(VIC_BASE + 0x14))
#define VICVECTADDR  (*(volatile uint32_t *)(VIC_BASE + 0x30))

/* SP804 Timer0 (Timers 0/1 share VIC interrupt line 4) */
#define TIMER0_BASE  0x101E2000u
#define TLOAD        (*(volatile uint32_t *)(TIMER0_BASE + 0x00))
#define TVALUE       (*(volatile uint32_t *)(TIMER0_BASE + 0x04))
#define TCTRL        (*(volatile uint32_t *)(TIMER0_BASE + 0x08))
#define TINTCLR      (*(volatile uint32_t *)(TIMER0_BASE + 0x0C))
#define TMIS         (*(volatile uint32_t *)(TIMER0_BASE + 0x14))

/* Timer control bits (SP804) */
#define TCTRL_ON         (1u << 7)
#define TCTRL_PERIODIC   (1u << 6)
#define TCTRL_INT_ENABLE (1u << 5)
#define TCTRL_32BIT      (1u << 1)

/* IRQ line for Timers 0/1 on VersatilePB primary VIC is bit 4. */
#define IRQ_TIMER01_BIT  (1u << 4)

/* Default timer clock at reset is 32KHz on this platform; 5s ≈ 5*32768 ticks. */
#define TIMER_HZ   32768u
#define TICKS_5S   (5u * TIMER_HZ)

static void uart_putc(char c) {
    while (UARTFR & UARTFR_TXFF) {}
    UARTDR = (uint32_t)c;
}

static void uart_puts(const char *s) {
    while (*s) uart_putc(*s++);
}

static void uart_init(void) {
    UARTCR = 0;
    UARTIBRD = 13;
    UARTFBRD = 1;
    UARTLCRH = (3u << 5); /* 8N1 */
    UARTCR = (1u << 9) | (1u << 8) | 1u; /* TXE|RXE|UARTEN */
}

static inline void enable_irqs(void) {
    __asm__ volatile (
        "mrs r0, cpsr\n"
        "bic r0, r0, #0x80\n"   /* clear I bit */
        "msr cpsr_c, r0\n"
        :::"r0","memory");
}

static void vic_init_for_timer(void) {
    VICINTENCLEAR = 0xFFFFFFFFu;
    VICINTENABLE  = IRQ_TIMER01_BIT;
}

static void timer_init_5s_tick(void) {
    TCTRL = 0;
    TLOAD = TICKS_5S;
    TINTCLR = 1; /* clear any pending */
    TCTRL = TCTRL_32BIT | TCTRL_PERIODIC | TCTRL_INT_ENABLE | TCTRL_ON;
}

/* Called from startup.s irq_handler */
void c_irq_handler(void) {
    /* Timers 0/1 share VIC line 4; check timer MIS to confirm */
    if (TMIS & 1u) {
        TINTCLR = 1;      /* clear timer interrupt */
        uart_putc('*');   /* proof-of-life */
    }
    VICVECTADDR = 0;      /* EOI */
}

int main(void) {
    uart_init();
    uart_puts("\nCASE5: timer IRQ + UART\n");

    vic_init_for_timer();
    timer_init_5s_tick();
    enable_irqs();

    for (;;) {
		__asm__ volatile ("nop");
    }
}