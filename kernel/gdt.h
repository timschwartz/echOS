#pragma once

typedef struct __attribute__((__packed__))
{
    uint16_t limit;
    uint64_t base;
} gdt_desc;
