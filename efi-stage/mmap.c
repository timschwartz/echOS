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

void stackPages(efi_mmap_t mmap, uint8_t type, stack64_t *pages)
{
    uint64_t offset = mmap.start;
    EFI_MEMORY_DESCRIPTOR *desc = NULL;
    uint64_t counter, address;

    while(offset < mmap.end)
    {
        desc = (EFI_MEMORY_DESCRIPTOR *)offset;

        if(desc->Type != type)
        {
            offset += mmap.descriptorSize;
            continue;
        }

        if(desc->PhysicalStart == (uint64_t)pages->stack)
        {
            offset += mmap.descriptorSize;
            continue;
        }

        counter = 1;
        address = desc->PhysicalStart + (desc->NumberOfPages * 4096);
        while(counter < desc->NumberOfPages + 1)
        {
            stack64_push(pages, address - (counter * 4096));
            counter++;
        }

        offset += mmap.descriptorSize;
    }

    offset = mmap.start;
    while(offset < mmap.end)
    {
        desc = (EFI_MEMORY_DESCRIPTOR *)offset;

        if(desc->Type != type)
        {
            offset += mmap.descriptorSize;
            continue;
        }

        if(desc->PhysicalStart != (uint64_t)pages->stack)
        {
            offset += mmap.descriptorSize;
            continue;
        }

        counter = 1;
        address = desc->PhysicalStart + (desc->NumberOfPages * 4096);
        while(counter < desc->NumberOfPages + 1)
        {
            stack64_push(pages, address - (counter * 4096));
            counter++;
        }

        offset += mmap.descriptorSize;
    }
}

EFI_STATUS setupPageStack(efi_mmap_t mmap, stack64_t *pages)
{
    const uint8_t type = 7;
    uint32_t bytes = getPageCountByType(mmap, type) * 8;
    pages->size = bytes / 4096;
    if((pages->size * 4096) < bytes) pages->size++;

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

        if(desc->NumberOfPages >= pages->size)
        {
            pages->stack = (uint64_t *)desc->PhysicalStart;
            pages->sp = 0;
            stackPages(mmap, type, pages);
            pages->sp -= pages->size;
            return EFI_SUCCESS;
        }
        offset += mmap.descriptorSize;
    }

    return EFI_NOT_FOUND;
}

uint64_t *getFreePage(stack64_t *pages)
{
    return (uint64_t *)stack64_pop(pages);
}
