#pragma once
#include <efi.h>
#include <efilib.h>

EFI_STATUS efi_fread(CHAR16 *filename, size_t *length, uint8_t **buffer);
