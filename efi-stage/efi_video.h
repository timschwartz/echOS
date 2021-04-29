#pragma once
#include <efi.h>
#include <efilib.h>

EFI_GRAPHICS_OUTPUT_PROTOCOL *get_gop(void);
EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *get_graphics_info(void);

void draw_border(size_t width, size_t offset, uint32_t pixel);
void setup_ssfn(CHAR16 *filename);
