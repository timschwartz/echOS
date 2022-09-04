#include <string.h>
#include "kernel.h"
#include "drivers/ssfn_fb.h"

void kernel_start(colonel_t system)
{
    ssfn_setup(system.fb);
    ssfn_set_color(0xFFFFFFFF, 0);
    ssfn_printf(system.fb, "Starting kernel...\n");

    /* Setup GDT */
    // Null segment
    gdt_entry e = gdt_entry_create(0, 0, 0, 0);
    gdt_entry_add(system.gdt, 0, e);

    // Kernel code segment
    e = gdt_entry_create(0, 0xFFFFF, 0x9A, 0xA);
    gdt_entry_add(system.gdt, 1, e);

    // Kernel data segment
    e = gdt_entry_create(0, 0xFFFFF, 0x92, 0xC);
    gdt_entry_add(system.gdt, 2, e);

    // User code segment
    e = gdt_entry_create(0, 0xFFFFF, 0xFA, 0xA);
    gdt_entry_add(system.gdt, 3, e);

    // User data segment
    e = gdt_entry_create(0, 0xFFFFF, 0xF2, 0xC);
    gdt_entry_add(system.gdt, 4, e);

    gdt_set(*(system.gdt));
    ssfn_printf(system.fb, "Setup GDT at 0x%x\n", system.gdt->base);
    /* End setup GDT */

    for(;;) __asm__ ("hlt");

    ssfn_printf(system.fb, "You escaped somehow\n");
}
