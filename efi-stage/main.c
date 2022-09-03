#include <efi.h>
#include <efilib.h>
#include "efi_pmm.h"
#include "efi_video.h"
#include "efi_fs.h"
#include "efi_malloc.h"
#include "../kernel/kernel.h"
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

    draw_border(5, 0, blue);

    ssfn_setup(L"\\EFI\\boot\\unifont.sfn", 10);
    ssfn_printf(PACKAGE_STRING);

    ssfn_set_color(cyan, 0);
    ssfn_printf(" EFI bootloader\n\n");

    ssfn_set_color(white, 0);

    colonel_t system;
    system.rsdp2 = get_rsdp2(SystemTable);
    if(init_pmm(SystemTable, &system) != EFI_SUCCESS)
    {
        ssfn_printf("Could not initialize physical memory map.\n");
        goto hang;
    }
    ssfn_printf("Setup physical memory map.\n");
    system.gdt = gdt_init(5, &efi_malloc);
    system.printf = &ssfn_printf;

    uefi_call_wrapper((void *)SystemTable->BootServices->ExitBootServices, 2, ImageHandle, system.mmap_key);
    kernel_start(system);

    ssfn_printf("You shouldn't be here.\n");

hang:
    for(;;);

    return result;
}
