#pragma once
#include <stdint.h>

typedef uint64_t gdt_entry;

typedef struct __attribute__((__packed__))
{
    uint16_t limit;
    uint64_t base;
} gdt_desc;

gdt_desc gdt_get();
void gdt_set(gdt_desc);
gdt_desc *gdt_init(size_t count, void *(*malloc)(uint64_t));
gdt_entry gdt_entry_create(uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);
void gdt_entry_add(gdt_desc *, uint8_t index, gdt_entry);

#if 0
const uint64_t GDT_ACCESSED_BIT = 0x1;
const uint64_t GDT_RW_BIT = 0x2;
const uint64_t GDT_DC_BIT = 0x4;
const uint64_t GDT_EXEC_BIT = 0x8;
const uint64_t GDT_TYPE_BIT = 0x10;

const uint64_t GDT_DPL_KERNEL = 0x0;
const uint64_t GDT_DPL_USER = 0x3;
#endif
