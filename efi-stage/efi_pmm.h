#pragma once
#include <efi.h>
#include <efilib.h>
#include "../kernel/kernel.h"

EFI_STATUS init_pmm(EFI_SYSTEM_TABLE *SystemTable, pmm **physical_memory, uint64_t *mmap_key);
