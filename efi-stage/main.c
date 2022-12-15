#include <efi.h>
#include <efilib.h>
#include "efi_pmm.h"
#include "efi_video.h"
#include "efi_fs.h"
#include "efi_malloc.h"
#include "efi_mmap.h"
#include "../kernel/kernel.h"
#include "../kernel/drivers/ssfn_fb.h"
#include "../config.h"

void display_error(EFI_STATUS result)
{
    switch(result)
    {
        case EFI_SUCCESS:
            Print(L"EFI_SUCCESS\n");
            break;
        case EFI_INVALID_PARAMETER:
            Print(L"EFI_INVALID_PARAMETER\n");
            break;
        case EFI_UNSUPPORTED:
            Print(L"EFI_UNSUPPORTED\n");
            break;
        case EFI_BUFFER_TOO_SMALL:
            Print(L"EFI_BUFFER_TOO_SMALL\n");
            break;
        default:
           Print(L"Unknown result = 0x%x\n", result);
           break;
    }
}

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

    /* Load font into EFI memory */
    uint8_t *efi_font_file;
    size_t font_size;
    if(efi_fread(L"\\EFI\\boot\\unifont.sfn", &font_size, &efi_font_file) != EFI_SUCCESS)
    {   
        Print(L"Couldn't open \\EFI\\boot\\unifont.sfn\n");
        goto hang;
    }

    uint8_t *efi_kernel_file;
    size_t kernel_size;
    if(efi_fread(L"\\EFI\\boot\\libkernel.so", &kernel_size, &efi_kernel_file) != EFI_SUCCESS)
    {   
        Print(L"Couldn't open \\EFI\\boot\\libkernel.so\n");
        goto hang;
    }

    colonel_t *system = efi_malloc(sizeof(colonel_t));

    if((system->rsdp2 = get_rsdp2(SystemTable)) == NULL)
    {
        Print(L"Could not get pointer to RSDP2.\n");
        goto hang;
    }

    /* Initialize physical memory manager in EFI memory */
    uint64_t mmap_key;
    if(init_pmm(ImageHandle, SystemTable, &(system->physical_memory)) != EFI_SUCCESS)
    {
        Print(L"Could not initialize physical memory map.\n");
        goto hang;
    }

    system->fb = framebuffer_init(10);
    system->fb.font_size = font_size;
    system->fb.font = efi_font_file;

    kernel_start(system);
hang:
    Print(L"Hanging now.\n");
    for(;;)
    {
        __asm__ ("hlt");
    }

    return result;
}
