// case4/gfx.h
#pragma once
#include <stdint.h>

static inline uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b)
{
    // pack as BGR565
    return (uint16_t)(((b & 0xF8) << 8) |
                      ((g & 0xFC) << 3) |
                      ((r & 0xF8) >> 3));
}

void put_px565(volatile uint16_t *fb, int stride_pixels, int x, int y, uint16_t c);
void fill_rect565(volatile uint16_t *fb, int stride_pixels, int x, int y, int w, int h, uint16_t c);