#include <efi.h>
#include <efilib.h>
#include <string.h>
#include "mmap.h"

const uint64_t kernel_size = 5;

extern void kernel_start(stack64_t *pageStack);
void (*kernel_entry)(stack64_t *pageStack);

EFI_STATUS
EFIAPI
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);

    EFI_STATUS result = -1;

    uefi_call_wrapper(ST->ConOut->ClearScreen, 1, ST->ConOut);
    SystemTable->BootServices->SetWatchdogTimer(0, 0, 0, NULL);

    efi_mmap_t mmap = { 0, 0, 0, 0, 0 };
    if((result = getEFIMemoryMap(SystemTable, &mmap)) != EFI_SUCCESS)
    {
        Print(L"Error getting EFI memory map.");
        return result;
    }

    stack64_t pages;
    setupPageStack(mmap, &pages);

    // Copy stack to memory
    uint64_t *memPtr = getFreePage(&pages);
    stack64_t *pageStack = (stack64_t *)memPtr;
    memset(pageStack, 0, 4096);
    //for(uint16_t i = 0; i < 4096; i++) memPtr[i] = 0;
    pageStack->stack = pages.stack;
    pageStack->size = pages.size;
    pageStack->sp = pages.sp;

    Print(L"Page stack\n  Address: 0x%X\n  Pages free:  %d\n  Size: %d pages / %d KB\n", pages.stack, pages.sp, pages.size, pages.size * 4);
    Print(L"Page stack\n  Address: 0x%X\n  Pages free:  %d\n  Size: %d pages / %d KB\n", pageStack->stack, pageStack->sp, pageStack->size, pageStack->size * 4);
    Print(L"Address of pageStack: 0x%X\n", pageStack);

//    Print(L"Function size: %d bytes\n", sizeof(getEFIMemoryMap));
//    uefi_call_wrapper((void *)SystemTable->BootServices->ExitBootServices, 2, ImageHandle, mmap.key);
    for(;;);

    return result;
}
