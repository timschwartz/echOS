#include <string.h>
#include "kernel.h"

void kernel_start(colonel_t system)
{
    system.printf("Hello from kernel\n");

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
    system.printf("Setup GDT at 0x%x\n", system.gdt->base);
    /* End setup GDT */

    for(;;);

    system.printf("You escaped somehow\n");
}
