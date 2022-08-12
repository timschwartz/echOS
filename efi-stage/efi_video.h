#pragma once
#include <efi.h>
#include <efilib.h>

EFI_GRAPHICS_OUTPUT_PROTOCOL *get_gop(void);
EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *get_graphics_info(void);

void draw_border(size_t width, size_t offset, uint32_t pixel);
void ssfn_setup(CHAR16 *filename, uint16_t margin);
void ssfn_set_color(uint32_t foreground, uint32_t background);
void ssfn_printf(char *message);
