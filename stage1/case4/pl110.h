// case4/pl110.h
#pragma once
#include <stdint.h>

#define PL110_BASE      0x10120000u

#define CLCD_TIM0       (*(volatile uint32_t *)(PL110_BASE + 0x00))
#define CLCD_TIM1       (*(volatile uint32_t *)(PL110_BASE + 0x04))
#define CLCD_TIM2       (*(volatile uint32_t *)(PL110_BASE + 0x08))
#define CLCD_TIM3       (*(volatile uint32_t *)(PL110_BASE + 0x0C))
#define CLCD_UPBASE     (*(volatile uint32_t *)(PL110_BASE + 0x10))
#define CLCD_LPBASE     (*(volatile uint32_t *)(PL110_BASE + 0x14))

/*
 * VersatilePB quirk:
 * CONTROL is at 0x18 (and IMSC at 0x1C). :contentReference[oaicite:1]{index=1}
 */
#define CLCD_CONTROL    (*(volatile uint32_t *)(PL110_BASE + 0x18))
#define CLCD_IMSC       (*(volatile uint32_t *)(PL110_BASE + 0x1C))

/* Known-good VGA timing values seen used on versatilepb. :contentReference[oaicite:2]{index=2} */
#define CLCD_VGATIM0     0x3F1F3F9Cu
#define CLCD_VGATIM1     0x000B61DFu   // VBP = 0, keep the rest
#define CLCD_VGATIM2     0x067F1800u

/* Control bits (common PL110-style definitions) */
#define CLCD_CTRL_LCDEN      (1u << 0)
#define CLCD_CTRL_BPP(n)     ((uint32_t)(n) << 1)   /* bits [3:1] */
#define CLCD_CTRL_TFT        (1u << 5)
#define CLCD_CTRL_BGR        (1u << 8)
#define CLCD_CTRL_BEBO       (1u << 9)
#define CLCD_CTRL_BEPO       (1u << 10)
#define CLCD_CTRL_PWR        (1u << 11)             /* LCD power enable described in PL110-style docs :contentReference[oaicite:3]{index=3} */

/* BPP field values (PL110: 16bpp is typically 0b100) :contentReference[oaicite:4]{index=4} */
#define CLCD_BPP_16          4u
#define CLCD_BPP_24          5u