#pragma once
#include <stdint.h>

typedef struct
{
    size_t width, height;
    size_t pixels_per_scanline;
    size_t margin;
    uint8_t *buffer;

    uint8_t *font;
    size_t font_size;
    
} framebuffer_t;
