#include <efi.h>
#include <efilib.h>
#include "efi_pmm.h"
#include "efi_video.h"
#include "efi_fs.h"
#include "efi_malloc.h"
#include "../kernel/kernel.h"
#include "../kernel/drivers/ssfn_fb.h"
#include "../config.h"

#include <elf.h>

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

    system.rsdp2 = get_rsdp2(SystemTable);
    if(init_pmm(SystemTable, &system) != EFI_SUCCESS)
    {
        Print(L"Could not initialize physical memory map.\n");
        goto hang;
    }

    uint8_t *kernel_raw;
    size_t kernel_size;
    if(efi_fread(L"\\EFI\\boot\\kernel", &kernel_size, &kernel_raw) != EFI_SUCCESS)
    {
        Print(L"Couldn't open \\EFI\\boot\\kernel\n");
        goto hang;
    }

    Print(L"Kernel is %d bytes at 0x%x\n", kernel_size, kernel_raw);

    Elf64_Ehdr *header = (Elf64_Ehdr *)kernel_raw;
    Print(L"Section header is at 0x%x\n", header->e_shoff);
    uefi_call_wrapper((void *)SystemTable->BootServices->ExitBootServices, 2, ImageHandle, system.mmap_key);
    //kernel_start(system);

    Print(L"You shouldn't be here.\n");

hang:
    for(;;)
    {
        __asm__ ("hlt");
    }

    return result;
}
