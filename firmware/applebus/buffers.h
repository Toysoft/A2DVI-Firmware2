/*
MIT License

Copyright (c) 2021 Mark Aikens
Copyright (c) 2023 David Kuder
Copyright (c) 2024 Thorsten Brehm

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include <stdint.h>

extern volatile uint32_t reset_counter;
extern volatile uint32_t bus_counter;
extern volatile uint32_t bus_overflow_counter;
extern volatile uint32_t frame_counter;
extern volatile uint32_t devicereg_counter;
extern volatile uint32_t devicerom_counter;
extern volatile uint32_t vblank_counter;

extern volatile uint16_t last_address;
extern volatile uint16_t last_address_stack;
extern volatile uint16_t last_address_pc;
extern volatile uint16_t last_address_zp;

#ifdef FEATURE_TEST
extern          uint32_t boot_time;
#endif

extern volatile uint8_t  reset_state;

extern volatile uint8_t  cardslot;
extern volatile uint16_t card_rom_address;

#define MAX_ADDRESS (0xC000)

extern uint8_t apple_memory[MAX_ADDRESS];
extern uint8_t private_memory[MAX_ADDRESS];

extern uint8_t status_line[4*40]; // 4 rows of 40 columns

extern volatile uint8_t jumpers;

extern volatile uint8_t *text_p1;
extern volatile uint8_t *text_p2;
extern volatile uint8_t *text_p3;
extern volatile uint8_t *text_p4;
extern volatile uint8_t *hgr_p1;
extern volatile uint8_t *hgr_p2;
extern volatile uint8_t *hgr_p3;
extern volatile uint8_t *hgr_p4;

/* Videx VideoTerm */
extern volatile uint8_t *videx_page;

#define apple_tbcolor  apple_memory[0xC022]
#define apple_border   apple_memory[0xC034]

#if 0
extern volatile uint8_t *baseio;
extern volatile uint8_t *slotio;
extern volatile uint8_t *slotrom;
extern volatile uint8_t *extdrom;
#endif

extern volatile uint32_t soft_switches;

extern volatile uint32_t internal_flags;

#define SOFTSW_TEXT_MODE      0x00000001ul
#define SOFTSW_MIX_MODE       0x00000002ul
#define SOFTSW_HIRES_MODE     0x00000004ul
#define SOFTSW_MODE_MASK      0x00000007ul
#define SOFTSW_PAGE_2         0x00000008ul

// Apple IIe/c/gs softswitches
#define SOFTSW_CXROM          0x00000010ul
#define SOFTSW_80STORE        0x00000100ul
#define SOFTSW_AUX_READ       0x00000200ul
#define SOFTSW_AUX_WRITE      0x00000400ul
#define SOFTSW_AUXZP          0x00000800ul
#define SOFTSW_SLOT3ROM       0x00001000ul
#define SOFTSW_80COL          0x00002000ul
#define SOFTSW_ALTCHAR        0x00004000ul
#define SOFTSW_DGR            0x00008000ul

#define SOFTSW_NEWVID_MASK    0xE0ul
#define SOFTSW_NEWVID_SHIFT   11

#define SOFTSW_MONOCHROME     0x00010000ul
//#define SOFTSW_LINEARIZE      0x00020000ul
//#define SOFTSW_SHR            0x00040000ul

#define SOFTSW_IOUDIS         0x00080000ul

// V2 Analog specific softswitches
#define IFLAGS_DEBUG_LINES    0x00100000ul
#define IFLAGS_MENU_ENABLE    0x00200000ul
#define IFLAGS_FORCED_MONO    0x00400000ul
#define IFLAGS_SCANLINEEMU    0x00800000ul
//#define IFLAGS_INTERP         0x01000000ul
//#define IFLAGS_GRILL          0x02000000ul
#define IFLAGS_VIDEO7         0x04000000ul
//#define IFLAGS_OLDCOLOR       0x08000000ul
//#define IFLAGS_TERMINAL       0x10000000ul
#define IFLAGS_TEST           0x20000000ul
#define IFLAGS_IIE_REGS       0x40000000ul
#define IFLAGS_IIGS_REGS      0x80000000ul

#define IFLAGS_V7_MODE0       0x00000000ul
#define IFLAGS_V7_MODE1       0x00000001ul
#define IFLAGS_V7_MODE2       0x00000002ul
#define IFLAGS_V7_MODE3       0x00000003ul

// size of a single character set
#define CHARACTER_ROM_SIZE    2048

// charater ROM for US + local character set
extern uint8_t character_rom[2*CHARACTER_ROM_SIZE];
extern uint8_t custom_font_buffer[2*CHARACTER_ROM_SIZE];

#define IS_IFLAG(FLAGS)             ((internal_flags & FLAGS)==FLAGS)
#define SET_IFLAG(condition, FLAGS) { if (condition) internal_flags |= FLAGS;else internal_flags &= ~FLAGS; }

#define IS_SOFTSWITCH(FLAGS)        ((soft_switches & FLAGS)==FLAGS)
