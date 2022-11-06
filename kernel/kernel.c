#include <string.h>
#include "kernel.h"
#include "drivers/ssfn_fb.h"
#include "../config.h"

colonel_t *system;

const size_t gdt_entry_count = 5;

void kernel_start(colonel_t *sys)
{
    system = sys;

    ssfn_setup(system->fb);
    ssfn_set_color(0xFFFFFFFF, 0);
    ssfn_printf(system->fb, "%s\n\n", PACKAGE_STRING);
    ssfn_printf(system->fb, "Copied physical memory map to 0x%x.\n", system->physical_memory);
    ssfn_printf(system->fb, "kernel_start() is at 0x%llx.\n", kernel_start);
    ssfn_printf(system->fb, "Starting kernel...\n");

    /* Start setup GDT */
    uint64_t pPage = frame_allocate(system->physical_memory);
    ssfn_printf(system->fb, "Allocated frame at 0x%llx.\n", pPage);
    system->gdt = (gdt_desc *)pPage;
    pPage += sizeof(gdt_desc);
    ssfn_printf(system->fb, "Created GDT descriptor at 0x%x.\n", system->gdt);

    system->gdt->limit = gdt_entry_count * sizeof(gdt_entry) - 1;
    system->gdt->base = pPage;
    pPage += system->gdt->limit + 1;

    // Null segment
    gdt_entry e = gdt_entry_create(0, 0, 0, 0);
    gdt_entry_add(system->gdt, 0, e);

    // Kernel code segment
    e = gdt_entry_create(0, 0xFFFFF, 0x9A, 0xA);
    gdt_entry_add(system->gdt, 1, e);

    // Kernel data segment
    e = gdt_entry_create(0, 0xFFFFF, 0x92, 0xC);
    gdt_entry_add(system->gdt, 2, e);

    // User code segment
    e = gdt_entry_create(0, 0xFFFFF, 0xFA, 0xA);
    gdt_entry_add(system->gdt, 3, e);

    // User data segment
    e = gdt_entry_create(0, 0xFFFFF, 0xF2, 0xC);
    gdt_entry_add(system->gdt, 4, e);

    gdt_set(*(system->gdt));
    ssfn_printf(system->fb, "Setup GDT at 0x%x.\n", system->gdt->base);
    /* End setup GDT */

    system->pml4 = init_pml4(system->physical_memory);
//    map_page(system->physical_memory, system->pml4, 0x2000, 0x2000, PRESENT_BIT | READ_WRITE_BIT);

    ssfn_printf(system->fb, "Created kernel PML4 at 0x%x.\n", system->pml4);

    for(;;) __asm__ ("hlt");

    ssfn_printf(system->fb, "You escaped somehow\n");
}
