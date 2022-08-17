#include "gdt.h"

gdt_desc get_gdt()
{
    gdt_desc gdt;
    __asm__ __volatile__ (
    "sgdt %0"
    : "=m" (gdt)
    : /* no input */
    : "memory"
    );

    return gdt;
}
