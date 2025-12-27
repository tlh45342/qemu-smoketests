// case4/main.c
#include <stdint.h>
#include "pl110.h"
#include "gfx.h"
#include "icon_spinner.h"

/* Put framebuffer somewhere safe in RAM (aligned enough for DMA). */
#define FB_ADDR   0x00200000u

#define SCREEN_W  640
#define SCREEN_H  480

/* Very dumb delay (we’ll replace with timer IRQ later) */
static void delay(volatile uint32_t n)
{
    while (n--) __asm__ volatile("nop");
}

static void pl110_init_vga_16bpp(uint32_t fb_addr)
{
    /* Program timing. Values used successfully on versatilepb. :contentReference[oaicite:5]{index=5} */
    CLCD_TIM0 = CLCD_VGATIM0;
    CLCD_TIM1 = CLCD_VGATIM1;
    CLCD_TIM2 = CLCD_VGATIM2;
    CLCD_TIM3 = 0;

    /* Framebuffer base */
    CLCD_UPBASE = fb_addr;
    CLCD_LPBASE = fb_addr;

    /*
     * Enable TFT + 16bpp + power.
     * Bit meanings (incl. power enable) are described in PL110-style docs. :contentReference[oaicite:6]{index=6}
     */
    uint32_t ctrl = 0;
    ctrl |= CLCD_CTRL_LCDEN;
    ctrl |= CLCD_CTRL_TFT;
    ctrl |= CLCD_CTRL_PWR;
    ctrl |= CLCD_CTRL_BPP(CLCD_BPP_16);
    CLCD_CONTROL = ctrl;
}

static void blit_spinner16_565(volatile uint16_t *fb, int stride, int x, int y,
                               int frame, uint16_t fg, uint16_t bg, int scale)
{
    /* clear sprite area */
    fill_rect565(fb, stride, x, y, ICON_W * scale, ICON_H * scale, bg);

    const uint16_t *rows = kSpinner16[frame & (ICON_FRAMES - 1)];
    for (int yy = 0; yy < ICON_H; yy++) {
        uint16_t bits = rows[yy];
        for (int xx = 0; xx < ICON_W; xx++) {
            if (bits & (1u << (15 - xx))) {
                for (int sy = 0; sy < scale; sy++) {
                    for (int sx = 0; sx < scale; sx++) {
                        put_px565(fb, stride, x + xx * scale + sx, y + yy * scale + sy, fg);
                    }
                }
            }
        }
    }
}

int main(void)
{
    volatile uint16_t *fb = (volatile uint16_t *)(uintptr_t)FB_ADDR;

    pl110_init_vga_16bpp(FB_ADDR);

    const int stride = SCREEN_W;

    /* Background */
    uint16_t bg = rgb565(0, 0, 0);
    fill_rect565(fb, stride, 0, 0, SCREEN_W, SCREEN_H, bg);

    /* Spinner colors */
    uint16_t fg = rgb565(255, 220, 0);

    int frame = 0;
    int scale = 6;

    int sw = ICON_W * scale;
    int sh = ICON_H * scale;

    // Start near the top-left (inside the border)
    int x = 20;
    int y = 20;

    int vx = 1;   // velocity in pixels/frame
    int vy = 1;

    // Keep motion inside the border with a small inset
    const int inset = 4;
    const int xmin = 2 + inset;
    const int ymin = 2 + inset;
    const int xmax = SCREEN_W - 2 - inset - sw;
    const int ymax = SCREEN_H - 2 - inset - sh;

    /* Give a quick visual “we’re alive” border */
	fill_rect565(fb, stride, 0, 0, SCREEN_W, 2, rgb565(255, 0, 0));                 // top
	fill_rect565(fb, stride, 0, SCREEN_H-2, SCREEN_W, 2, rgb565(0, 255, 0));        // bottom
	fill_rect565(fb, stride, 0, 0, 2, SCREEN_H, rgb565(0, 128, 255));               // left
	fill_rect565(fb, stride, SCREEN_W-2, 0, 2, SCREEN_H, rgb565(255, 0, 255));      // right

    int px = x, py = y;

    for (;;) {
        // Erase previous sprite area only (dirty rect)
        fill_rect565(fb, stride, px, py, sw, sh, bg);

        // Draw new frame at current position
        blit_spinner16_565(fb, stride, x, y, frame, fg, bg, scale);

        // Save current as previous for next iteration
        px = x; py = y;

        // Advance animation frame
        frame = (frame + 1) & (ICON_FRAMES - 1);

        // Move
        x += vx;
        y += vy;

        // Bounce off edges
        if (x <= xmin) { x = xmin; vx = -vx; }
        if (x >= xmax) { x = xmax; vx = -vx; }
        if (y <= ymin) { y = ymin; vy = -vy; }
        if (y >= ymax) { y = ymax; vy = -vy; }

        // Tune speed
        delay(900000);
    }
}