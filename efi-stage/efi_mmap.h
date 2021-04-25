#include <stdint.h>
#include <efi.h>
#include <efilib.h>
#include "../kernel/memory/physical.h"

typedef struct efi_mmap_t
{ 
    uint64_t start;
    uint64_t end;
    uint64_t size;
    uint64_t key;
    uint64_t descriptorSize;
} efi_mmap_t;

EFI_STATUS getEFIMemoryMap(EFI_SYSTEM_TABLE *SystemTable, efi_mmap_t *mmap);
uint32_t getPageCountByType(efi_mmap_t mmap, uint8_t type);
size_t countPhysicalMemoryBlocks(efi_mmap_t mmap);
EFI_STATUS setupPhysicalMemoryBlock(efi_mmap_t mmap, pm_block *block, const size_t block_index);
