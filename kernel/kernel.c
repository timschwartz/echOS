#include <string.h>
#include "kernel.h"
#include "drivers/ssfn_fb.h"
#include "../config.h"

colonel_t *system;

const size_t gdt_entry_count = 5;

/* Build page tables that identity map every region the kernel may still
   touch, then switch to them: RAM of every kind the firmware reported (the
   kernel image, its stack, the PMM, boot data and the page tables all live
   there), ACPI and runtime regions, and the framebuffer. Page 0 stays
   unmapped so NULL dereferences fault. */
static int init_paging(void)
{
    pmm *pm = system->physical_memory;

    system->pml4 = init_pml4(pm);
    if(system->pml4 == NULL) return -1;

    for(size_t i = 0; i < pm->block_count; i++)
    {
        pm_block *block = pm->blocks[i];
        if(block->type == PM_MMIO || block->type == PM_RESERVED) continue;

        uint64_t start = block->address;
        uint64_t end = start + block->frames_total * frame_size;
        if(start < frame_size) start = frame_size;
        if(start >= end) continue;

        if(map_range(pm, system->pml4, start, start, end - start, READ_WRITE_BIT) != 0) return -1;
    }

    uint64_t fb_start = (uint64_t)system->fb.buffer;
    uint64_t fb_length = system->fb.pixels_per_scanline * system->fb.height * 4;
    if(map_range(pm, system->pml4, fb_start, fb_start, fb_length, READ_WRITE_BIT) != 0) return -1;

    set_cr3((uint64_t)system->pml4);
    return 0;
}

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
    gdt_desc *gdt = (gdt_desc *)frame_allocate(system->physical_memory);
    if(gdt == 0xFFFFFFFFFFFFFFFF)
    {
        ssfn_printf(system->fb, "Failed to allocate GDT descriptor.\n");
        for(;;) __asm__ ("hlt");
    }

    system->gdt = gdt;
    ssfn_printf(system->fb, "Created GDT descriptor at 0x%x.\n", system->gdt);

    system->gdt->limit = gdt_entry_count * sizeof(gdt_entry) - 1;

    uint64_t base = frame_allocate(system->physical_memory);
    if(base == 0xFFFFFFFFFFFFFFFF)
    {
        ssfn_printf(system->fb, "Failed to allocate GDT base.\n");
        for(;;) __asm__ ("hlt");
    }
    system->gdt->base = base;

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

    gdt_flush(system->gdt);

    ssfn_printf(system->fb, "Setup GDT at 0x%x, limit: 0x%x.\n", system->gdt->base, system->gdt->limit);
    /* End setup GDT */

    if(init_paging() != 0)
    {
        ssfn_printf(system->fb, "Failed to build kernel page tables.\n");
        for(;;) __asm__ ("hlt");
    }
    ssfn_printf(system->fb, "Loaded kernel PML4 at 0x%llx.\n", system->pml4);

    for(;;) __asm__ ("hlt");

    ssfn_printf(system->fb, "You escaped somehow\n");
}
