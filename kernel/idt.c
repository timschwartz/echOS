#include "idt.h"
#include "kernel.h"
#include "drivers/ssfn_fb.h"
#include <string.h>

idt_desc_t idt_get()
{
    idt_desc_t idt;
    __asm__ __volatile__ (
    "sidt %0"
    : "=m" (idt)
    : /* no input */
    : "memory"
    );

    return idt;
}

void idt_set(idt_desc_t idt)
{
    __asm__ __volatile__ (
    "lidt %0"
    :
    : "m" (idt)
    : "memory"
    );
}

idt_desc_t *idt_init(void *(*malloc)(uint64_t))
{
    size_t length = sizeof(idt_entry_t) * 256;

    idt_desc_t *idt = (idt_desc_t *)malloc(sizeof(idt_desc_t));
    idt->limit = length - 1;

    idt->base = (uint64_t)malloc(length);

    memset((uint64_t *)(idt->base), 0, length);

    return idt;
}

idt_entry_t idt_entry_create(uint64_t isr, uint16_t segment, uint8_t ist, uint8_t flags)
{
    idt_entry_t entry;

    entry.isr1 = (uint16_t)(isr & 0xFFFF);
    entry.isr2 = (uint16_t)((isr >> 16) & 0xFFFF);
    entry.isr3 = (uint32_t)(isr >> 32);

    entry.segment = segment;
    entry.ist = ist;
    entry.flags = flags;
    return entry;
}

void idt_entry_add(idt_desc_t *desc, uint8_t index, idt_entry_t entry)
{
    idt_entry_t *ptr = (idt_entry_t *)desc->base;
    ptr[index] = entry;
}

void dump_idt_entry(idt_entry_t entry)
{
    uint64_t isr = entry.isr3;
             isr <<= 32;
             isr |= entry.isr2 << 16;
             isr |= entry.isr1;
    ssfn_printf(system->fb, "isr = 0x%x, segment = 0x%x, ist = 0x%x, flags = 0x%x\n", isr, entry.segment, entry.ist, entry.flags);
}

void dump_idt(idt_desc_t idt)
{
    size_t count = (idt.limit + 1) / sizeof(idt_entry_t);
    ssfn_printf(system->fb, "IDTR Base: 0x%x, Limit: 0x%x\n", idt.base, idt.limit);

    for(uint16_t counter = 0; counter < count; counter++)
    {
        idt_entry_t *entry = (idt_entry_t *)(idt.base + (counter * sizeof(idt_entry_t)));
        ssfn_printf(system->fb, "IDT %d: ", counter);
        dump_idt_entry(*entry);
    }
}

