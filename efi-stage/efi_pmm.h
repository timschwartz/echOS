#pragma once
#include <efi.h>
#include <efilib.h>
#include "../kernel/kernel.h"

EFI_STATUS init_pmm(EFI_SYSTEM_TABLE *SystemTable, colonel_t *system);
