#include "gdt.h"
#include <string.h>

gdt_desc gdt_get()
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

void gdt_set(gdt_desc *gdt)
{
    __asm__ __volatile__ (
    "cli\n"
    "mov %0, %%rdi\n"
    "lgdt (%%rdi)\n"
    "mov $0x10, %%ax\n"
    "mov %%ax, %%ds\n"
    "mov %%ax, %%es\n"
    "mov %%ax, %%ss\n"
    :
    : "m" (gdt)
    : "memory"
    );

    gdt_flush();
}

gdt_desc *gdt_init(size_t count, void *(*malloc)(uint64_t))
{
    size_t length = count * 8;

    gdt_desc *gdt = (gdt_desc *)malloc(sizeof(gdt_desc));
    gdt->limit = length - 1;

    gdt->base = (uint64_t)malloc(length);

    memset((uint64_t *)(gdt->base), 0, length);

    return gdt;
}

gdt_entry gdt_entry_create(uint32_t base, uint32_t limit, uint8_t access, uint8_t flags)
{
    gdt_entry entry;

    uint64_t base1 = base >> 24;
    uint64_t base2 = (base >> 16) & 0xFF;
    uint64_t base3 = base & 0xFFFF;

    uint64_t limit1 = (limit >> 16) & 0xF;
    uint64_t limit2 = limit & 0xFFFF;

    entry = base1 << 56;
    entry |= ((uint64_t)flags & 0xF) << 52;
    entry |= limit1 << 48;
    entry |= (uint64_t)access << 40;
    entry |= base2 << 32;
    entry |= base3 << 16;
    entry |= limit2;

    return entry;
}

void gdt_entry_add(gdt_desc *desc, uint8_t index, gdt_entry entry)
{
    gdt_entry *ptr = (gdt_entry *)desc->base;
    ptr[index] = entry;
}
