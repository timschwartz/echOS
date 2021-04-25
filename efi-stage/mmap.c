#include "mmap.h"

EFI_STATUS getEFIMemoryMap(EFI_SYSTEM_TABLE *SystemTable, efi_mmap_t *mmap)
{
    EFI_STATUS result = -1;

    EFI_MEMORY_DESCRIPTOR *memoryMap = NULL;
    UINT32 descriptorVersion = 1;

    while(EFI_SUCCESS != (result = uefi_call_wrapper((void *)SystemTable->BootServices->GetMemoryMap, 5, &(mmap->size),
                                                   memoryMap, &(mmap->key), &(mmap->descriptorSize), &descriptorVersion)))
    {
        if(result == EFI_BUFFER_TOO_SMALL)
        {
            mmap->size += 2 * mmap->descriptorSize;
            uefi_call_wrapper((void *)SystemTable->BootServices->AllocatePool, 3, EfiLoaderData, mmap->size, (void **)&memoryMap);
        }
        else return result;
    }

    mmap->start = (uint64_t)memoryMap;
    mmap->end = mmap->start + mmap->size;

    return result;
}

uint32_t getPageCountByType(efi_mmap_t mmap, uint8_t type)
{
    uint64_t offset = mmap.start;
    uint32_t totalPages = 0;
    EFI_MEMORY_DESCRIPTOR *desc = NULL;
    while(offset < mmap.end)
    {
        desc = (EFI_MEMORY_DESCRIPTOR *)offset;

        if(desc->Type != type)
        {
            offset += mmap.descriptorSize;
            continue;
        }

        Print(L"Physical start: 0x%lx, Virtual start: 0x%lx    %d pages\n", desc->PhysicalStart, desc->VirtualStart, desc->NumberOfPages);
        totalPages += desc->NumberOfPages;
        offset += mmap.descriptorSize;
    }

    return totalPages;
}

size_t countPhysicalMemoryBlocks(efi_mmap_t mmap)
{
    size_t result = 0;
    const uint8_t type = 7;

    uint64_t offset = mmap.start;
    EFI_MEMORY_DESCRIPTOR *desc = NULL;
    while(offset < mmap.end)
    {
        desc = (EFI_MEMORY_DESCRIPTOR *)offset;

        if(desc->Type != type)
        {
            offset += mmap.descriptorSize;
            continue;
        }

        result++;

        offset += mmap.descriptorSize;
    }

    return result;
}

EFI_STATUS setupPhysicalMemoryBlock(efi_mmap_t mmap, pm_block *block, const size_t block_index)
{
    const uint8_t type = 7;

    uint64_t offset = mmap.start;
    size_t counter = 0;
    EFI_MEMORY_DESCRIPTOR *desc = NULL;
    while(offset < mmap.end)
    {
        desc = (EFI_MEMORY_DESCRIPTOR *)offset;

        if(desc->Type != type)
        {
            offset += mmap.descriptorSize;
            continue;
        }

        if(block_index == counter)
        {
            block->frames_total = block->frames_free = desc->NumberOfPages;
            block->address = desc->PhysicalStart;
            return EFI_SUCCESS;
        }

        counter++;
        if(counter > block_index) return EFI_NOT_FOUND;
        offset += mmap.descriptorSize;
    }
    return EFI_NOT_FOUND;
}
