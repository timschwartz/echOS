#pragma once
#include <efi.h>
#include <efilib.h>

EFI_STATUS EFIAPI efi_fopen(EFI_SYSTEM_TABLE *SystemTable, EFI_FILE *file, CHAR16 *filename);
EFI_STATUS EFIAPI efi_fread(EFI_FILE *file, uint8_t *buffer);
