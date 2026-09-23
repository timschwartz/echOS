#pragma once
#include <efi.h>
#include <efilib.h>
#include "../kernel/kernel.h"
#include "efi_mmap.h"

/* Storage for the PMM, allocated while Boot Services are still available and
   filled in from the final memory map after ExitBootServices. */
typedef struct
{
    pmm *physical_memory;
    size_t block_capacity;
    uint64_t *bitmap;
    size_t bitmap_words;
} pmm_storage;

EFI_STATUS reserve_pmm(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable, pmm_storage *storage);
void build_pmm(pmm_storage *storage, efi_mmap_t mmap);
void dump_pmm(pmm *);
