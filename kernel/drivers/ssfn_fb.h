#pragma once
#include "../framebuffer.h"
#include <stddef.h>

void ssfn_setup(framebuffer_t fb);
void ssfn_set_color(uint32_t foreground, uint32_t background);
int ssfn_printf(framebuffer_t fb, char *format, ...);
