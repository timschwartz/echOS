#include <string.h>
#include "kernel.h"
#include "drivers/ssfn_fb.h"
#include "../config.h"

colonel_t *system;

const size_t gdt_entry_count = 5;

void kernel_start(colonel_t sys)
{
    uint64_t pPage = frame_allocate(sys.physical_memory);

    system = (colonel_t *)pPage;
    pPage += sizeof(colonel_t);
    system->fb.width = sys.fb.width;
    system->fb.height = sys.fb.height;
    system->fb.pixels_per_scanline = sys.fb.pixels_per_scanline;
    system->fb.margin = sys.fb.margin;
    system->fb.buffer = sys.fb.buffer;
    system->fb.font_size = sys.fb.font_size;

    {
        size_t frames_needed = system->fb.font_size / frame_size;
        if(frames_needed * frame_size < system->fb.font_size) frames_needed++;
        system->fb.font = (uint8_t *)frames_allocate(sys.physical_memory, frames_needed);
        memset(system->fb.font, 0, frames_needed * frame_size);
        memcpy(system->fb.font, sys.fb.font, system->fb.font_size);
    }

    ssfn_setup(system->fb);
    ssfn_set_color(0xFFFFFFFF, 0);
    ssfn_printf(sys.fb, "%s\n\n", PACKAGE_STRING);
    ssfn_printf(sys.fb, "Starting kernel...\n");

    pPage = frame_allocate(sys.physical_memory);
    system->physical_memory = (pmm *)pPage;
    pPage += sizeof(pmm);
    system->physical_memory->block_count = sys.physical_memory->block_count;

    for(size_t i = 0; i < system->physical_memory->block_count; i++)
    {
        pm_block *b1 = sys.physical_memory->blocks[i];
        size_t size = frame_map_size(b1->frames_total) * 8;

        size_t frames_needed = size / frame_size;
        if(frames_needed * frame_size < size) frames_needed++;
        pm_block *b2 = (pm_block *)frames_allocate(sys.physical_memory, frames_needed);
        memset(b2, 0, frames_needed * frame_size);
        memcpy(b2, b1, size);
        system->physical_memory->blocks[i] = b2;
    }

    ssfn_printf(sys.fb, "Copied physical memory map to 0x%x.\n", system->physical_memory);

    pPage = frame_allocate(system->physical_memory);
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

    for(;;) __asm__ ("hlt");

    ssfn_printf(system->fb, "You escaped somehow\n");
}
