#pragma once
#include <stdint.h>

typedef uint64_t gdt_entry;

typedef struct __attribute__((__packed__))
{
    uint16_t limit;
    uint64_t base;
} gdt_desc;

gdt_desc gdt_get();
void gdt_set(gdt_desc *);
gdt_desc *gdt_init(size_t count, void *(*malloc)(uint64_t));
gdt_entry gdt_entry_create(uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);
void gdt_entry_add(gdt_desc *, uint8_t index, gdt_entry);
extern void gdt_flush();
