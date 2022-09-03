#include "efi_pmm.h"
#include "efi_mmap.h"
#include "efi_malloc.h"

EFI_STATUS init_pmm(EFI_SYSTEM_TABLE *SystemTable, colonel_t *system)
{
    efi_mmap_t mmap = { 0, 0, 0, 0, 0 };
    if(getEFIMemoryMap(SystemTable, &mmap) != EFI_SUCCESS)
    {
        Print(L"Error getting EFI memory map.\n");
        return 0;
    }

    pmm *physical_memory = (pmm *)efi_malloc(sizeof(pmm));

    physical_memory->block_count = countPhysicalMemoryBlocks(mmap);
    physical_memory->blocks = (pm_block **)efi_malloc(sizeof(pm_block) * physical_memory->block_count);

    for(size_t i = 0; i < physical_memory->block_count; i++)
    {
        pm_block *block = (pm_block *)efi_malloc(sizeof(pm_block));
        physical_memory->blocks[i] = block;
        setupPhysicalMemoryBlock(mmap, block, i);

        uint64_t map_size = frame_map_size(block->frames_total);
        if(map_size < 1) map_size++;

        block->map = (uint64_t *)efi_malloc(sizeof(uint64_t) * map_size);
        for(size_t j = 0; j < map_size; j++) block->map[j] = 0;
    }

    system->physical_memory = physical_memory;
    system->mmap_key = mmap.key;
    return EFI_SUCCESS;
}
