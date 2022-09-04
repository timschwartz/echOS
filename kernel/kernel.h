#pragma once

#include "memory/physical.h"
#include "acpi/acpi.h"
#include "gdt.h"
#include "framebuffer.h"

typedef struct
{
    size_t count;
    madt_processor *apic_data;
} cpu_info_t;

typedef struct
{
    pmm *physical_memory;
    gdt_desc *gdt;
    rsdp2_desc *rsdp2;
    cpu_info_t cpus;
    framebuffer_t fb;

    uint64_t mmap_key;
} colonel_t;

void kernel_start(colonel_t);
