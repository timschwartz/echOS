#include "efi_mmap.h"

EFI_STATUS getEFIMemoryMap(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable, efi_mmap_t *mmap)
{
    EFI_STATUS result = -1;

    EFI_MEMORY_DESCRIPTOR *memoryMap = NULL;
    UINT32 descriptorVersion = 1;

    while(EFI_SUCCESS != (result = uefi_call_wrapper(SystemTable->BootServices->GetMemoryMap, 5, &(mmap->size),
                                                   memoryMap, &(mmap->key), &(mmap->descriptorSize), &descriptorVersion)))
    {
        if(result != EFI_BUFFER_TOO_SMALL) break;

        if(memoryMap != NULL)
        {
            uefi_call_wrapper(SystemTable->BootServices->FreePool, 1, memoryMap);
            memoryMap = NULL;
        }

        mmap->size += 2 * mmap->descriptorSize;
        result = uefi_call_wrapper(SystemTable->BootServices->AllocatePool, 3, EfiLoaderData, mmap->size, (void **)&memoryMap);
        if(result != EFI_SUCCESS)
        {
            memoryMap = NULL;
            break;
        }
    }

    if(result != EFI_SUCCESS)
    {
        if(memoryMap != NULL) uefi_call_wrapper(SystemTable->BootServices->FreePool, 1, memoryMap);
        mmap->start = mmap->end = 0;
        return result;
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
