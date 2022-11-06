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

    pmm *physical_memory, *efi_physical_memory;
    uint64_t mmap_key;
    if(init_pmm(SystemTable, &efi_physical_memory, &mmap_key) != EFI_SUCCESS)
    {
        Print(L"Could not initialize physical memory map.\n");
        goto hang;
    }

    /* Copy pmm out of EFI memory. */
    physical_memory = (pmm *)frame_allocate(efi_physical_memory);
    physical_memory->block_count = efi_physical_memory->block_count;

    for(size_t i = 0; i < physical_memory->block_count; i++)
    {
        pm_block *b1 = efi_physical_memory->blocks[i];
        size_t map_size = frame_map_size(b1->frames_total);
        if(map_size < 1) map_size++;

        size_t frames_needed = map_size / frame_size;
        if(frames_needed * frame_size < map_size) frames_needed++;
        pm_block *b2 = (pm_block *)frames_allocate(efi_physical_memory, frames_needed);

        b2->address = b1->address;
        b2->frames_total = b1->frames_total;

        Print(L"Trying to allocate %d bytes.\n", sizeof(uint64_t) * map_size);
        b2->map = (uint64_t *)frames_allocate(efi_physical_memory, sizeof(uint64_t) * map_size);

        physical_memory->blocks[i] = b2;
    }

    Print(L"Allocated space for physical memory map.\n");

    for(size_t i = 0; i < physical_memory->block_count; i++)
    {
        pm_block *b1 = efi_physical_memory->blocks[i];
        pm_block *b2 = physical_memory->blocks[i];

        uint64_t map_size = frame_map_size(b2->frames_total);
        if(map_size < 1) map_size++;
        memcpy(b2->map, b1->map, map_size);

        b2->frames_free = b1->frames_free;

        Print(L"Filled in memory map %d at 0x%llx. %d frames total. %d frames free.\n", 
            i, b2->map, b2->frames_total, b2->frames_free);
    }

    colonel_t *system = (colonel_t *)frame_allocate(physical_memory);
    system->physical_memory = physical_memory;
    Print(L"system=0x%llx\n", system);
    Print(L"system->physical_memory=0x%llx\n", system->physical_memory);

    uint8_t *efi_kernel_file;
    size_t kernel_size;
    if(efi_fread(L"\\EFI\\boot\\libkernel.so", &kernel_size, &efi_kernel_file) != EFI_SUCCESS)
    {   
        Print(L"Couldn't open \\EFI\\boot\\libkernel.so\n");
        goto hang;
    }

    Print(L"Copied kernel (%d bytes) to 0x%llx.\n", kernel_size, efi_kernel_file);

    system->fb = framebuffer_init(10);
    uint8_t *font_file;
    if(efi_fread(L"\\EFI\\boot\\unifont.sfn", &system->fb.font_size, &font_file) != EFI_SUCCESS)
    {
        Print(L"Couldn't open \\EFI\\boot\\unifont.sfn\n");
        goto hang;
    }

    Print(L"Copied font (%d bytes) to 0x%llx.\n", system->fb.font_size, font_file);

    /* Copy font out of EFI memory */
    {
        size_t frames_needed = system->fb.font_size / frame_size;
        if(frames_needed * frame_size < system->fb.font_size) frames_needed++;
        Print(L"Need %d frames for font\n", frames_needed);
        frames_allocate(system->physical_memory, 2);
Print(L"Success\n");
        system->fb.font = (uint8_t *)frames_allocate(system->physical_memory, frames_needed);
//        Print(L"system->fb.font = 0x%llx.\n", system->fb.font);
Print(L"Hi\n");
        memset(system->fb.font, 0, frames_needed * frame_size);
        memcpy(system->fb.font, font_file, system->fb.font_size);
    }

    Print(L"Copied font (%d bytes) to 0x%llx.\n", system->fb.font_size, system->fb.font);

    if((system->rsdp2 = get_rsdp2(SystemTable)) == NULL)
    {
        Print(L"Could not get pointer to RSDP2.\n");
        goto hang;
    }

    uefi_call_wrapper((void *)SystemTable->BootServices->ExitBootServices, 2, ImageHandle, mmap_key);

#if 0
    /* Copy kernel out of EFI memory */
    uint8_t *kernel;
    {
        size_t frames_needed = kernel_size / frame_size;
        if(frames_needed * frame_size < kernel_size) frames_needed++;
        kernel = (uint8_t *)frames_allocate(system->physical_memory, frames_needed);
        Print(L"Allocated %d frames\n", frames_needed);
        memset(kernel, 0, frames_needed * frame_size);
        memcpy(kernel, &efi_kernel_file, kernel_size);

        Print(L"Copied kernel to 0x%llx\n", kernel);
    }
#endif 

//    kernel_start(system);

    Print(L"You shouldn't be here.\n");

hang:
    Print(L"Hanging up now.\n");
    for(;;)
    {
        __asm__ ("hlt");
    }

    return result;
}
