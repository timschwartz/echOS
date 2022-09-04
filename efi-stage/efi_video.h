#pragma once
#include <efi.h>
#include <efilib.h>
#include "../kernel/framebuffer.h"

EFI_GRAPHICS_OUTPUT_PROTOCOL *get_gop(void);
EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *get_graphics_info(void);
framebuffer_t framebuffer_init(size_t margin);
