#include <stdint.h>
#include <stddef.h>

/* ========= PL110 / versatilepb basics ========= */

#define SYS_OSCCLK4      (*(volatile uint32_t *)0x1000001Cu)

#define LCD_BASE         0x10120000u
#define LCD_TIM0         (*(volatile uint32_t *)(LCD_BASE + 0x00))
#define LCD_TIM1         (*(volatile uint32_t *)(LCD_BASE + 0x04))
#define LCD_TIM2         (*(volatile uint32_t *)(LCD_BASE + 0x08))
#define LCD_UPBASE       (*(volatile uint32_t *)(LCD_BASE + 0x10))
#define LCD_CONTROL      (*(volatile uint32_t *)(LCD_BASE + 0x18))

// Safe framebuffer address (same as your working TGA viewer)
#define FB_ADDR          0x01000000u

#define WIDTH            800u
#define HEIGHT           600u

// PL110: 32bpp, 0x00BBGGRR
static volatile uint32_t *const fb = (volatile uint32_t *)FB_ADDR;

static inline void put_pixel(uint32_t x, uint32_t y, uint32_t color)
{
    if (x >= WIDTH || y >= HEIGHT) return;
    fb[y * WIDTH + x] = color;
}

static void clear_screen(uint32_t color)
{
    for (uint32_t i = 0; i < WIDTH * HEIGHT; ++i) {
        fb[i] = color;
    }
}

static void draw_filled_rect(uint32_t x0, uint32_t y0,
                             uint32_t x1, uint32_t y1,
                             uint32_t color)
{
    if (x1 < x0 || y1 < y0) return;

    if (x1 >= WIDTH)  x1 = WIDTH - 1;
    if (y1 >= HEIGHT) y1 = HEIGHT - 1;

    for (uint32_t y = y0; y <= y1; ++y) {
        for (uint32_t x = x0; x <= x1; ++x) {
            put_pixel(x, y, color);
        }
    }
}

static void lcd_init_800x600(void)
{
    SYS_OSCCLK4 = 0x2CAC;
    LCD_TIM0    = 0x1313A4C4;
    LCD_TIM1    = 0x0505F657;
    LCD_TIM2    = 0x071F1800;

    LCD_UPBASE  = FB_ADDR;
    LCD_CONTROL = 0x82B;
}

/* ========= Simple 8×8 bitmap font ========= */

#define FONT_W  8u
#define FONT_H  8u

typedef struct {
    char    ch;
    uint8_t rows[FONT_H];
} Glyph;

static const Glyph font[] = {
    // Space ' '
    { ' ', { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 } },

    // 'A'..'Z'
    { 'A', { 0x18,0x24,0x42,0x42,0x7E,0x42,0x42,0x00 } },
    { 'B', { 0x7C,0x42,0x42,0x7C,0x42,0x42,0x7C,0x00 } },
    { 'C', { 0x3C,0x42,0x40,0x40,0x40,0x42,0x3C,0x00 } },
    { 'D', { 0x78,0x44,0x42,0x42,0x42,0x44,0x78,0x00 } },
    { 'E', { 0x7E,0x40,0x40,0x7C,0x40,0x40,0x7E,0x00 } },
    { 'F', { 0x7E,0x40,0x40,0x7C,0x40,0x40,0x40,0x00 } },
    { 'G', { 0x3C,0x42,0x40,0x4E,0x42,0x42,0x3C,0x00 } },
    { 'H', { 0x42,0x42,0x42,0x7E,0x42,0x42,0x42,0x00 } },
    { 'I', { 0x3E,0x08,0x08,0x08,0x08,0x08,0x3E,0x00 } },
    { 'J', { 0x1E,0x04,0x04,0x04,0x44,0x44,0x38,0x00 } },
    { 'K', { 0x42,0x44,0x48,0x70,0x48,0x44,0x42,0x00 } },
    { 'L', { 0x40,0x40,0x40,0x40,0x40,0x40,0x7E,0x00 } },
    { 'M', { 0x42,0x66,0x5A,0x5A,0x42,0x42,0x42,0x00 } },
    { 'N', { 0x42,0x62,0x52,0x4A,0x46,0x42,0x42,0x00 } },
    { 'O', { 0x3C,0x42,0x42,0x42,0x42,0x42,0x3C,0x00 } },
    { 'P', { 0x7C,0x42,0x42,0x7C,0x40,0x40,0x40,0x00 } },
    { 'Q', { 0x3C,0x42,0x42,0x42,0x4A,0x44,0x3A,0x00 } },
    { 'R', { 0x7C,0x42,0x42,0x7C,0x48,0x44,0x42,0x00 } },
    { 'S', { 0x3C,0x42,0x40,0x3C,0x02,0x42,0x3C,0x00 } },
    { 'T', { 0x7F,0x49,0x08,0x08,0x08,0x08,0x1C,0x00 } },
    { 'U', { 0x42,0x42,0x42,0x42,0x42,0x42,0x3C,0x00 } },
    { 'V', { 0x42,0x42,0x42,0x42,0x42,0x24,0x18,0x00 } },
    { 'W', { 0x42,0x42,0x42,0x5A,0x5A,0x66,0x42,0x00 } },
    { 'X', { 0x42,0x42,0x24,0x18,0x24,0x42,0x42,0x00 } },
    { 'Y', { 0x42,0x42,0x24,0x18,0x08,0x08,0x1C,0x00 } },
    { 'Z', { 0x7E,0x02,0x04,0x18,0x20,0x40,0x7E,0x00 } },
};

static const unsigned font_count = sizeof(font) / sizeof(font[0]);

static const Glyph *find_glyph(char c)
{
    if (c >= 'a' && c <= 'z') {
        c = (char)(c - 'a' + 'A');
    }
    if (c == ' ')
        return &font[0]; // space is first

    for (unsigned i = 1; i < font_count; ++i) {
        if (font[i].ch == c)
            return &font[i];
    }
    return NULL;
}

static void draw_char(uint32_t x, uint32_t y,
                      char c,
                      uint32_t fg, uint32_t bg)
{
    const Glyph *g = find_glyph(c);
    if (!g) g = find_glyph(' '); // fallback to space

    for (uint32_t row = 0; row < FONT_H; ++row) {
        uint8_t bits = g->rows[row];
        for (uint32_t col = 0; col < FONT_W; ++col) {
            uint32_t mask  = 1u << (7u - col);
            uint32_t color = (bits & mask) ? fg : bg;
            put_pixel(x + col, y + row, color);
        }
    }
}

static void draw_string(uint32_t x, uint32_t y,
                        const char *s,
                        uint32_t fg, uint32_t bg)
{
    uint32_t cx = x;
    while (*s) {
        draw_char(cx, y, *s++, fg, bg);
        cx += FONT_W;
    }
}

/* ========= Demo main ========= */

int main(void)
{
    lcd_init_800x600();

    const uint32_t BLACK  = 0x00000000u;
    const uint32_t RED    = 0x000000FFu;
    const uint32_t GREEN  = 0x0000FF00u;
    const uint32_t BLUE   = 0x00FF0000u;
    const uint32_t WHITE  = 0x00FFFFFFu;
    // const uint32_t YELLOW = 0x0000FFFFu;

    clear_screen(BLACK);

    // Three big rectangles as before
    draw_filled_rect(20,  40, 120, 100, RED);
    draw_filled_rect(20, 140, 120, 200, GREEN);
    draw_filled_rect(20, 240, 120, 300, BLUE);

    // Draw "HELLO" at (200, 60)
    const char *text = "HELLO";
    uint32_t tx = 200;
    uint32_t ty = 60;
    draw_string(tx, ty, text, WHITE, BLACK);

    for (;;) {
        // idle
    }
}
