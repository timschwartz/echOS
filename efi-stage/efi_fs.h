#pragma once
#include <efi.h>
#include <efilib.h>

//EFI_STATUS EFIAPI
uint8_t *efi_fread(CHAR16 *filename, size_t *length);
