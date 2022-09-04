#include <efi.h>
#include <efilib.h>
#include "efi_pmm.h"
#include "efi_video.h"
#include "efi_fs.h"
#include "efi_malloc.h"
#include "../kernel/kernel.h"
#include "../kernel/drivers/ssfn_fb.h"
#include "../config.h"

const uint32_t cyan = 0x0000FFFF;
const uint32_t blue = 0x000070FF;
const uint32_t white = 0xFFFFFFFF;
const uint32_t black = 0x00000000;

EFI_GUID ACPI2 = ACPI_20_TABLE_GUID;

rsdp2_desc *get_rsdp2(EFI_SYSTEM_TABLE *SystemTable)
{
    for(uint16_t counter = 0; counter < SystemTable->NumberOfTableEntries; counter++)
    {
        EFI_CONFIGURATION_TABLE table = SystemTable->ConfigurationTable[counter];
        if(CompareGuid(&ACPI2, &table.VendorGuid)) continue;

        return (rsdp2_desc *)table.VendorTable;
    }
    return NULL;
}

EFI_STATUS
EFIAPI
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);

    EFI_STATUS result = -1;

    uefi_call_wrapper(ST->ConOut->ClearScreen, 1, ST->ConOut);
    SystemTable->BootServices->SetWatchdogTimer(0, 0, 0, NULL);

    colonel_t system;
    system.fb = framebuffer_init(10);
    if(efi_fread(L"\\EFI\\boot\\unifont.sfn", &system.fb.font_size, &system.fb.font) != EFI_SUCCESS)
    {
        Print(L"Couldn't open \\EFI\\boot\\unifont.sfn\n");
        goto hang;
    }

    Print(L"%a\n\n", PACKAGE_STRING);
    Print(L"EFI bootloader\n");

    system.rsdp2 = get_rsdp2(SystemTable);
    if(init_pmm(SystemTable, &system) != EFI_SUCCESS)
    {
        Print(L"Could not initialize physical memory map.\n");
        goto hang;
    }
    size_t frame_count = 0;
    for(size_t i = 0; i < system.physical_memory->block_count; i++)
    {
        frame_count += system.physical_memory->blocks[i]->frames_total;
    }
    size_t ram_size = (frame_count * 4096) / 1024 / 1024;
    Print(L"Initialized physical memory map...%dMB\n", ram_size);
    system.gdt = gdt_init(5, &efi_malloc);

    uefi_call_wrapper((void *)SystemTable->BootServices->ExitBootServices, 2, ImageHandle, system.mmap_key);
    kernel_start(system);

    Print(L"You shouldn't be here.\n");

hang:
    for(;;)
    {
        __asm__ ("hlt");
    }

    return result;
}
