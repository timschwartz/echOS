#include <efi.h>
#include <efilib.h>
#include <string.h>
#include "efi_pmm.h"
#include "efi_video.h"
#include "efi_fs.h"
#include "efi_random.h"
#include "../config.h"

const uint32_t cyan = 0x0000FFFF;
const uint32_t blue = 0x000070FF;
const uint32_t white = 0xFFFFFFFF;
const uint32_t black = 0x00000000;

typedef struct __attribute__((__packed__))
{
    uint16_t limit;
    uint64_t base;
} gdt_desc;

gdt_desc get_gdt()
{
    gdt_desc gdt;
    __asm__ __volatile__ (
    "sgdt %0"
    : "=m" (gdt)
    : /* no input */
    : "memory"
    );

    return gdt;
}

EFI_STATUS
EFIAPI
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);

    EFI_STATUS result = -1;

    uefi_call_wrapper(ST->ConOut->ClearScreen, 1, ST->ConOut);
    SystemTable->BootServices->SetWatchdogTimer(0, 0, 0, NULL);

    draw_border(10, 0, blue);

    ssfn_setup(L"\\EFI\\boot\\unifont.sfn", 20);
    ssfn_printf(PACKAGE_STRING);

    ssfn_set_color(0xFFFFFF00, 0xFFFF00FF);
    ssfn_printf(" EFI bootloader\n\n");

    ssfn_set_color(cyan, 0);
    gdt_desc gdt = get_gdt();
    Print (L"GDTR Base: 0x%X\n", gdt.base);
    Print (L"GDTR Limit: 0x%X\n", gdt.limit);

    for(uint16_t counter = 0; counter < ((gdt.limit + 1) / sizeof(uint64_t)); counter++)
    {
        uint64_t *desc = (uint64_t *)(gdt.base + (counter * sizeof(uint64_t)));
        Print (L"GDT %d: 0x%X\n", counter, *desc);
    }
//    uefi_call_wrapper((void *)SystemTable->BootServices->ExitBootServices, 2, ImageHandle, mmap.key);
    for(;;);

    return result;
}
