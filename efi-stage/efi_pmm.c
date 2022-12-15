#include "efi_pmm.h"
#include "efi_mmap.h"
#include "efi_malloc.h"

EFI_STATUS init_pmm(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable, pmm **pm)
{
    efi_mmap_t mmap = { 0, 0, 0, 0, 0 };
    if(getEFIMemoryMap(ImageHandle, SystemTable, &mmap) != EFI_SUCCESS)
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

        // Print(L"Creating memory block %d at 0x%llx. %d frames total. %d frames free.\n", i, block, block->frames_total, block->frames_free);

        uint64_t map_size = frame_map_size(block->frames_total);
        if(map_size < 1) map_size++;

        block->map = (uint64_t *)efi_malloc(map_size * sizeof(uint64_t));
        for(size_t j = 0; j < map_size; j++) block->map[j] = 0;
    }

    *pm = physical_memory;
    return EFI_SUCCESS;
}

void dump_pmm(pmm *pm)
{
    Print(L"Memory map at 0x%llx has %d blocks.\n", pm, pm->block_count);

    size_t total_frames = 0;
    for(size_t i = 0; i < pm->block_count; i++)
    {
        pm_block *b = pm->blocks[i];
        total_frames += b->frames_total;
        Print(L"Block %d: Address - 0x%llx, Phys. address - 0x%llx, total frames - %d, free frames - %d\n", i, b, b->address, b->frames_total, b->frames_free);
    }

    Print(L"Total frames: %d\n", total_frames);
}

void compare_pmm(pmm *a, pmm *b)
{
    Print(L"Comparing physical memory maps at a:0x%llx and b:0x%llx\n", a, b);
    if(a->block_count != b->block_count)
    {
        Print(L"Block count - a: %d, b: %d\n");
    }

    size_t block_count = a->block_count > b->block_count ? b->block_count : a->block_count;
    for(size_t i = 0; i < block_count; i++)
    {
        pm_block *a_block = a->blocks[i];
        pm_block *b_block = b->blocks[i];

        if(a_block->frames_total != b_block->frames_total)
        {
            Print(L"Block %d frames_total - a: %d, b: %d\n", i, a_block->frames_total, b_block->frames_total);
        }

        if(a_block->frames_free != b_block->frames_free)
        {
            Print(L"Block %d frames_free - a: %d, b: %d\n", i, a_block->frames_free, b_block->frames_free);
        }
    }
}
