// case4/gfx.c
#include "gfx.h"

void put_px565(volatile uint16_t *fb, int stride_pixels, int x, int y, uint16_t c)
{
    fb[y * stride_pixels + x] = c;
}

void fill_rect565(volatile uint16_t *fb, int stride_pixels, int x, int y, int w, int h, uint16_t c)
{
    for (int j = 0; j < h; j++) {
        volatile uint16_t *row = fb + (y + j) * stride_pixels + x;
        for (int i = 0; i < w; i++) row[i] = c;
    }
}