#pragma once
#include <stdint.h>

typedef struct __attribute__((__packed__))
{
    uint16_t isr1;
    uint16_t segment;
    uint8_t ist;
    uint8_t flags;
    uint16_t isr2;
    uint32_t isr3;
    uint32_t reserved;
} idt_entry_t;

typedef struct __attribute__((__packed__))
{
    uint16_t limit;
    uint64_t base;
} idt_desc_t;

idt_desc_t idt_get();
void idt_set(idt_desc_t);
idt_desc_t *idt_init(void *(*malloc)(uint64_t));
idt_entry_t idt_entry_create(uint64_t isr, uint16_t segment, uint8_t ist, uint8_t flags);
void idt_entry_add(idt_desc_t *, uint8_t index, idt_entry_t);
void dump_idt_entry(idt_entry_t entry);
void dump_idt(idt_desc_t idt);
