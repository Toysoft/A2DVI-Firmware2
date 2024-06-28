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

#include <pico/stdlib.h>
#include "applebus/buffers.h"
#include "config/config.h"
#include "render.h"
#include "hires_dot_patterns.h"
#include "hires_color_patterns_tmds.h"

#define PAGE2SEL ((soft_switches & (SOFTSW_80STORE | SOFTSW_PAGE_2)) == SOFTSW_PAGE_2)
uint16_t __attribute__((section(".uninitialized_data."))) lhalf_palette[16];

static uint32_t DELAYED_COPY_DATA(hires_mono_patterns_red)[4] = {
	TMDS_SYMBOL_0_0,
	TMDS_SYMBOL_0_0,
	TMDS_SYMBOL_0_0,
	TMDS_SYMBOL_0_0
};
static uint32_t DELAYED_COPY_DATA(hires_mono_patterns_green)[4] = {
	TMDS_SYMBOL_0_0,
	TMDS_SYMBOL_255_0,
	TMDS_SYMBOL_0_255,
	TMDS_SYMBOL_255_255
};
static uint32_t DELAYED_COPY_DATA(hires_mono_patterns_blue)[4] = {
	TMDS_SYMBOL_0_0,
	TMDS_SYMBOL_0_0,
	TMDS_SYMBOL_0_0,
	TMDS_SYMBOL_0_0
};

#if 0
static uint32_t DELAYED_COPY_DATA(hires_mono_patterns_half)[4] = {
	TMDS_SYMBOL_0_0,
	TMDS_SYMBOL_128_0,
	TMDS_SYMBOL_0_128,
	TMDS_SYMBOL_128_128
};

static uint32_t DELAYED_COPY_DATA(hires_mono_patterns_full)[4] = {
	TMDS_SYMBOL_0_0,
	TMDS_SYMBOL_255_0,
	TMDS_SYMBOL_0_255,
	TMDS_SYMBOL_255_255
};
#endif

static inline uint hires_line_to_mem_offset(uint line)
{
    return ((line & 0x07) << 10) | ((line & 0x38) << 4) | (((line & 0xc0) >> 6) * 40);
}

static void DELAYED_COPY_CODE(render_hires_line)(bool p2, uint line)
{
    const uint8_t *line_mem = (const uint8_t *)((p2 ? hgr_p2 : hgr_p1) + hires_line_to_mem_offset(line));

    dvi_get_scanline(tmdsbuf);
    dvi_scanline_rgb(tmdsbuf, tmdsbuf_red, tmdsbuf_green, tmdsbuf_blue);

    if(mono_rendering)
    {
        uint32_t lastmsb = 0;
        uint_fast8_t dotc = 0;
        uint32_t dots = 0;

        for(uint i=0; i < 40; i++)
        {
            // Load in as many subpixels as possible
            dots |= (hires_dot_patterns2[lastmsb | line_mem[i]]) << dotc;
            lastmsb = (dotc>0) ? ((line_mem[i] & 0x40)<<2) : 0;
            dotc += 14;

            // Consume pixels
            while(dotc)
            {
                *(tmdsbuf_red++)   = hires_mono_patterns_red[dots&0x3];
                *(tmdsbuf_green++) = hires_mono_patterns_green[dots&0x3];
                *(tmdsbuf_blue++)  = hires_mono_patterns_blue[dots&0x3];
                dots >>= 2;
                dotc -= 2;
            }
        }
    }
    else
    {
        // Each hires byte contains 7 pixels which may be shifted right 1/2 a pixel. That is
        // represented here by 14 'dots' to precisely describe the half-pixel positioning.
        //
        // For each pixel, inspect a window of 8 dots around the pixel to determine the
        // precise dot locations and colors.
        //
        // Dots would be scanned out to the CRT from MSB to LSB (left to right here):
        //
        //            previous   |        next
        //              dots     |        dots
        //        +-------------------+--------------------------------------------------+
        // dots:  | 31 | 30 | 29 | 28 | 27 | 26 | 25 | 24 | 23 | ... | 14 | 13 | 12 | ...
        //        |              |         |              |
        //        \______________|_________|______________/
        //                       |         |
        //                       \_________/
        //                         current
        //                          pixel
        uint oddness = 0;

        // Load in the first 14 dots
        uint32_t dots = (uint32_t)hires_dot_patterns[line_mem[0]] << 15;

        for(uint i=1; i < 41; i++)
        {
            // Load in the next 14 dots
            uint b = (i < 40) ? line_mem[i] : 0;
            if(b & 0x80) {
                // Extend the last bit from the previous byte
                dots |= (dots & (1u << 15)) >> 1;
            }
            dots |= (uint32_t)hires_dot_patterns[b] << 1;

            // Consume 14 dots
            for(uint j=0; j < 7; j++)
            {
                uint dot_pattern = oddness | ((dots >> 24) & 0xff);
                *(tmdsbuf_red++)   = hires_color_patterns_red[dot_pattern];
                *(tmdsbuf_green++) = hires_color_patterns_green[dot_pattern];
                *(tmdsbuf_blue++)  = hires_color_patterns_blue[dot_pattern];
                dots <<= 2;
                oddness ^= 0x100;
            }
        }
    }

    dvi_send_scanline(tmdsbuf);
}


void DELAYED_COPY_CODE(render_hires)()
{
    for(uint line=0; line < 192; line++) {
        render_hires_line(PAGE2SEL, line);
    }
}


void DELAYED_COPY_CODE(render_mixed_hires)()
{
    for(uint line=0; line < 160; line++) {
        render_hires_line(PAGE2SEL, line);
    }

    render_mixed_text();
}