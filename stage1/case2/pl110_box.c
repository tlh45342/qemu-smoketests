// pl110_box.c — QEMU versatilepb PL110 framebuffer demo (800x600x32)
// Draws a red square outline at (100,100)-(150,150) and loops forever.
//
// Run:
//   qemu-system-arm -M versatilepb -m 128M -kernel pl110_box.bin -display gtk

#include <stdint.h>

#define SYS_OSCCLK4      (*(volatile uint32_t *)0x1000001Cu)

#define LCD_BASE         0x10120000u
#define LCD_TIM0         (*(volatile uint32_t *)(LCD_BASE + 0x00))
#define LCD_TIM1         (*(volatile uint32_t *)(LCD_BASE + 0x04))
#define LCD_TIM2         (*(volatile uint32_t *)(LCD_BASE + 0x08))
#define LCD_UPBASE       (*(volatile uint32_t *)(LCD_BASE + 0x10))
#define LCD_CONTROL      (*(volatile uint32_t *)(LCD_BASE + 0x18))

// Framebuffer in RAM (must be in RAM; 1MB offset is a common choice)
#define FB_ADDR          0x00100000u

#define WIDTH            800u
#define HEIGHT           600u

// QEMU VersatilePB PL110 default pixel layout is commonly treated as 0x00BBGGRR.
// So "red" is 0x000000FF. :contentReference[oaicite:2]{index=2}
static volatile uint32_t *const fb = (volatile uint32_t *)FB_ADDR;

static inline void put_pixel(uint32_t x, uint32_t y, uint32_t color)
{
    if (x >= WIDTH || y >= HEIGHT) return;
    fb[y * WIDTH + x] = color;
}

static void clear_screen(uint32_t color)
{
    for (uint32_t i = 0; i < (WIDTH * HEIGHT); i++) {
        fb[i] = color;
    }
}

static void draw_rect_outline(uint32_t x0, uint32_t y0,
                              uint32_t x1, uint32_t y1,
                              uint32_t color)
{
    if (x1 < x0 || y1 < y0) return;

    for (uint32_t x = x0; x <= x1; x++) {
        put_pixel(x,  y0, color);
        put_pixel(x,  y1, color);
    }
    for (uint32_t y = y0; y <= y1; y++) {
        put_pixel(x0, y,  color);
        put_pixel(x1, y,  color);
    }
}

static void lcd_init_800x600(void)
{
    // Known-good VersatilePB/QEMU PL110 SVGA 800x600 timing values :contentReference[oaicite:3]{index=3}
    SYS_OSCCLK4 = 0x2CAC;
    LCD_TIM0    = 0x1313A4C4;
    LCD_TIM1    = 0x0505F657;
    LCD_TIM2    = 0x071F1800;

    LCD_UPBASE  = FB_ADDR;

    // Known-good control bits in QEMU’s PL110 model for this setup :contentReference[oaicite:4]{index=4}
    LCD_CONTROL = 0x082B;
}

__attribute__((naked, noreturn))
void _start(void)
{
    __asm__ volatile (
        // -m 128M => RAM ends at 0x08000000, so use that as stack top
        "ldr sp, =0x08000000 \n"
        "bl  main            \n"
        "b   .               \n"
    );
}

int main(void)
{
    lcd_init_800x600();

    const uint32_t BLACK = 0x00000000u;
    const uint32_t RED   = 0x000000FFu;   // 0x00BBGGRR => RR in low byte :contentReference[oaicite:5]{index=5}

    clear_screen(BLACK);
    draw_rect_outline(100, 100, 150, 150, RED);

    for (;;) { /* keep window alive */ }
}