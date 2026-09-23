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
        if(!CompareGuid(&ACPI2, &table.VendorGuid)) continue;

        return (rsdp2_desc *)table.VendorTable;
    }
    return NULL;
}

/* Called by gnu-efi's _entry with the SysV ABI, so no EFIAPI here. */
EFI_STATUS
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);

    EFI_STATUS result = -1;

    uefi_call_wrapper(ST->ConOut->ClearScreen, 1, ST->ConOut);
    SystemTable->BootServices->SetWatchdogTimer(0, 0, 0, NULL);

    uint8_t *font_file;
    size_t font_size;
    if(efi_fread(L"\\EFI\\boot\\unifont.sfn", &font_size, &font_file) != EFI_SUCCESS)
    {   
        Print(L"Couldn't open \\EFI\\boot\\unifont.sfn\n");
        goto hang;
    }

    uint8_t *kernel_file;
    size_t kernel_size;
    if(efi_fread(L"\\EFI\\boot\\libkernel.so", &kernel_size, &kernel_file) != EFI_SUCCESS)
    {   
        Print(L"Couldn't open \\EFI\\boot\\libkernel.so\n");
        goto hang;
    }

    colonel_t *system = efi_malloc(sizeof(colonel_t));
    if(system == NULL) goto hang;

    if((system->rsdp2 = get_rsdp2(SystemTable)) == NULL)
    {
        Print(L"Could not get pointer to RSDP2.\n");
        goto hang;
    }

    system->fb = framebuffer_init(10);
    system->fb.font_size = font_size;
    system->fb.font = font_file;

    /* Allocate the PMM storage. It's sized from a provisional memory map and
       filled in from the final one after ExitBootServices. */
    pmm_storage pmm_store;
    if(reserve_pmm(ImageHandle, SystemTable, &pmm_store) != EFI_SUCCESS)
    {
        Print(L"Could not reserve physical memory manager.\n");
        goto hang;
    }

    /* Allocate the final memory map buffer last, with slack for this
       allocation and for ExitBootServices retries. */
    efi_mmap_t mmap = { 0, 0, 0, 0, 0 };
    UINT32 descriptor_version;
    uefi_call_wrapper(SystemTable->BootServices->GetMemoryMap, 5, &(mmap.size), NULL,
                      &(mmap.key), &(mmap.descriptorSize), &descriptor_version);
    const uint64_t map_capacity = mmap.size + 8 * mmap.descriptorSize;
    EFI_MEMORY_DESCRIPTOR *map_buffer = efi_malloc(map_capacity);
    if(map_buffer == NULL) goto hang;

    /* No Print or allocation from here on: either would change the map and
       invalidate its key. */
    for(;;)
    {
        mmap.size = map_capacity;
        result = uefi_call_wrapper(SystemTable->BootServices->GetMemoryMap, 5, &(mmap.size), map_buffer,
                                   &(mmap.key), &(mmap.descriptorSize), &descriptor_version);

        result = uefi_call_wrapper(SystemTable->BootServices->ExitBootServices, 2, ImageHandle, mmap.key);
        if(result == EFI_SUCCESS) break;
        if(result != EFI_INVALID_PARAMETER) goto halt;   // anything else isn't a stale key
    }

    /* Boot Services are gone. The firmware's IDT is still loaded, so keep
       interrupts off until the kernel installs its own. */
    __asm__ volatile ("cli");

    mmap.start = (uint64_t)map_buffer;
    mmap.end = mmap.start + mmap.size;
    system->mmap_key = mmap.key;

    build_pmm(&pmm_store, mmap);
    system->physical_memory = pmm_store.physical_memory;

    kernel_start(system);
    goto halt;

hang:
    Print(L"Hanging now.\n");
halt:
    /* Failures after ExitBootServices come here directly, since Print no longer works. */
    for(;;)
    {
        __asm__ ("cli; hlt");
    }

    return result;
}

