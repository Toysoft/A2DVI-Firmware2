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

#include "buffers.h"

volatile uint32_t reset_counter;
volatile uint32_t bus_counter;
volatile uint32_t bus_overflow_counter;
volatile uint32_t frame_counter;
volatile uint32_t devicereg_counter;
volatile uint32_t devicerom_counter;
volatile uint32_t vblank_counter;

volatile uint16_t last_address;
volatile uint16_t last_address_stack;
volatile uint16_t last_address_pc;
volatile uint16_t last_address_zp;

#ifdef FEATURE_TEST
         uint32_t boot_time;
#endif

volatile uint32_t soft_switches = SOFTSW_TEXT_MODE;
volatile uint32_t internal_flags = IFLAGS_V7_MODE3;

volatile uint8_t  cardslot;
// Set SlotROM area to invalid address, so decoder does not trigger before the actual cardslot is determined.
volatile uint16_t card_rom_address = 0x00ff;

volatile uint8_t reset_state = 0;

uint8_t __attribute__((section (".appledata."))) apple_memory[MAX_ADDRESS];
uint8_t __attribute__((section (".appledata."))) private_memory[MAX_ADDRESS];

uint8_t __attribute__((section (".appledata."))) status_line[4*40]; // 4 rows of 40 columns

volatile uint8_t *text_p1 = apple_memory   + 0x0400;
volatile uint8_t *text_p2 = apple_memory   + 0x0800;
volatile uint8_t *text_p3 = private_memory + 0x0400;
volatile uint8_t *text_p4 = private_memory + 0x0800;
volatile uint8_t *hgr_p1  = apple_memory   + 0x2000;
volatile uint8_t *hgr_p2  = apple_memory   + 0x4000;
volatile uint8_t *hgr_p3  = private_memory + 0x2000;
volatile uint8_t *hgr_p4  = private_memory + 0x4000;

// The currently programmed character generator ROMs for text mode (US + local char set)
uint8_t __attribute__((section (".appledata."))) character_rom[2* CHARACTER_ROM_SIZE];

uint8_t __attribute__((section (".appledata."))) custom_font_buffer[2* CHARACTER_ROM_SIZE];
