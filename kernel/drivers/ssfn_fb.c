#include "ssfn_fb.h"
#include <stdarg.h>
#include <stdio.h>
#define SSFN_CONSOLEBITMAP_TRUECOLOR
#include <ssfn.h>

void ssfn_set_color(uint32_t foreground, uint32_t background)
{
    ssfn_dst.fg = foreground;
    ssfn_dst.bg = background;
}

void ssfn_setup(framebuffer_t fb)
{
    ssfn_src = (ssfn_font_t *)fb.font;
    ssfn_dst.ptr = fb.buffer;
    ssfn_dst.p = 4 * fb.pixels_per_scanline;
    ssfn_dst.x = fb.margin;
    ssfn_dst.y = fb.margin;

    ssfn_set_color(0xFFFFFFFF, 0x0);
}

void ssfn_put(framebuffer_t fb, char c)
{
    ssfn_putc(c);
    if(ssfn_dst.x > fb.width - fb.margin - 5) ssfn_putc('\n');
    if(ssfn_dst.x < fb.margin) ssfn_dst.x = fb.margin;
    if(ssfn_dst.y < fb.margin) ssfn_dst.y = fb.margin;
}

int ssfn_printf(framebuffer_t fb, char *format, ...)
{
    va_list values;
    va_start(values, format);

    char output[1024] = {0};
    int written = vsprintf(output, format, values);

    for(int i = 0; i < written; i++)
    {
        ssfn_put(fb, output[i]);
    }
    written++;
    return written;
}
