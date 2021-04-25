#include <efi.h>
#include <efilib.h>
#include <string.h>
#include "mmap.h"
#include "../kernel/memory/physical.h"
#include "../kernel/memory/paging.h"
#include "efi_video.h"

const uint64_t kernel_size = 5;

extern void kernel_start();
void (*kernel_entry)();

void *efi_malloc(UINTN poolSize);

EFI_STATUS
EFIAPI
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);

    EFI_STATUS result = -1;

    uefi_call_wrapper(ST->ConOut->ClearScreen, 1, ST->ConOut);
    SystemTable->BootServices->SetWatchdogTimer(0, 0, 0, NULL);

    Print(L"Hello there\n");

    efi_mmap_t mmap = { 0, 0, 0, 0, 0 };
    if((result = getEFIMemoryMap(SystemTable, &mmap)) != EFI_SUCCESS)
    {
        Print(L"Error getting EFI memory map.\n");
        return result;
    }

    /* Setup physical memory structures */
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

    /* Finished setting up physical memory structures */

    page_map_level_4 PML4 = init_pml4(physical_memory);

    Print(L"PML4 address: %lX\n", PML4);
    Print(L"  Entry 1: %lX\n", PML4[0]);

    page_directory_pointer_entry *pdp = get_pdp(PML4[0]);
    Print(L"PDP address: %lX\n", pdp);
    Print(L"  Entry 1: %lX\n", pdp[0]);

    page_directory_entry *pd = get_pd(pdp[0]);
    Print(L"PD address: %lX\n", pd);
    Print(L"  Entry 1: %lX\n", pd[0]);

    page_table_entry *pt = get_pt(pd[0]);
    Print(L"PT address: %lX\n", pt);
    Print(L"  Entry 2: %lX\n", pt[1]);

    uint64_t *vmem = (uint64_t *)(pt[1] & 0xFFFFFFFFFFFFF000);
    Print(L"Physical address of virtual address 0x0000000000001000: %lX\n", vmem);

    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = get_gop();

/*
    // Copy stack to memory
    uint64_t *memPtr = getFreePage(&pages);
    stack64_t *pageStack = (stack64_t *)memPtr;
    memset(pageStack, 0, 4096);
    pageStack->stack = pages.stack;
    pageStack->size = pages.size;
    pageStack->sp = pages.sp;
*/

//    uefi_call_wrapper((void *)SystemTable->BootServices->ExitBootServices, 2, ImageHandle, mmap.key);
    for(;;);

    return result;
}
