#include "paging.h"
#include "../kernel.h"
#include <stddef.h>
#include <string.h>

page_table_entry init_pt_entry(uint64_t paddr, uint64_t flags)
{
    page_table_entry entry = (page_table_entry)(paddr & PHYS_ADDR_MASK);
    entry |= flags;
    return entry;
}

page_table_entry *get_pt(page_directory_entry entry)
{
    return (page_table_entry *)(entry & PHYS_ADDR_MASK);
}

page_directory_entry *get_pd(page_directory_pointer_entry entry)
{
    return (page_directory_entry *)(entry & PHYS_ADDR_MASK);
}

page_directory_pointer_entry *get_pdp(page_map_level_4_entry entry)
{
    return (page_directory_pointer_entry *)(entry & PHYS_ADDR_MASK);
}

/* Allocate and zero one frame for a page table. Tables are reached by their
   physical address, so this relies on them being identity mapped. */
static uint64_t *alloc_table(pmm *physical_memory)
{
    uint64_t frame = frame_allocate(physical_memory);
    if(frame == 0xFFFFFFFFFFFFFFFF) return NULL;

    uint64_t *table = (uint64_t *)frame;
    memset(table, 0, frame_size);
    return table;
}

/* Return the table that table[index] points to, creating it if needed.
   Upper levels are always present and writable; the leaf entry decides the
   real permissions. Returns NULL if a frame can't be allocated or the entry
   is already a large page. */
static uint64_t *next_table(pmm *physical_memory, uint64_t *table, uint16_t index, uint64_t flags)
{
    if(!(table[index] & PRESENT_BIT))
    {
        uint64_t *next = alloc_table(physical_memory);
        if(next == NULL) return NULL;
        table[index] = (uint64_t)next | PRESENT_BIT | READ_WRITE_BIT | (flags & USER_SUPERVISOR_BIT);
        return next;
    }

    if(table[index] & PAGE_SIZE_BIT) return NULL;
    table[index] |= flags & USER_SUPERVISOR_BIT;
    return (uint64_t *)(table[index] & PHYS_ADDR_MASK);
}

page_map_level_4 init_pml4(pmm *physical_memory)
{
    return alloc_table(physical_memory);
}

/* Read control register CR<index>. Valid indexes are 0, 2, 3, 4 and 8;
   CR1 and the others don't exist and reading them raises #UD, so any other
   index returns 0xFFFFFFFFFFFFFFFF. volatile because CR2 and CR3 change
   underneath the compiler. */
uint64_t get_cr(size_t index)
{
    uint64_t value;
    switch(index)
    {
        case 0: __asm__ volatile ("mov %%cr0, %0" : "=r" (value)); return value;
        case 2: __asm__ volatile ("mov %%cr2, %0" : "=r" (value)); return value;
        case 3: __asm__ volatile ("mov %%cr3, %0" : "=r" (value)); return value;
        case 4: __asm__ volatile ("mov %%cr4, %0" : "=r" (value)); return value;
        case 8: __asm__ volatile ("mov %%cr8, %0" : "=r" (value)); return value;
        default: return 0xFFFFFFFFFFFFFFFF;
    }
}

void set_cr3(uint64_t entry)
{
    __asm__ __volatile__ (
        "mov %0, %%cr3"
        :
        : "r" (entry)
        : "memory"
    );
}

void *mmap(void *start, size_t length, int prot, int flags, 
           int fd, off_t offset)
{
    return (void *)0;
}

int map_page(pmm *physical_memory, page_map_level_4 pml4, uint64_t vaddr, uint64_t paddr, uint64_t flags)
{
    uint16_t index1 = (vaddr >> 12) & 0x1FF;
    uint16_t index2 = (vaddr >> 21) & 0x1FF;
    uint16_t index3 = (vaddr >> 30) & 0x1FF;
    uint16_t index4 = (vaddr >> 39) & 0x1FF;

    page_directory_pointer_entry *pdp = next_table(physical_memory, pml4, index4, flags);
    if(pdp == NULL) return -1;

    page_directory_entry *pd = next_table(physical_memory, pdp, index3, flags);
    if(pd == NULL) return -1;

    /* Already covered by a 2 MiB page: fine if it maps to the same frame. */
    if((pd[index2] & PRESENT_BIT) && (pd[index2] & PAGE_SIZE_BIT))
    {
        uint64_t mapped = (pd[index2] & PHYS_ADDR_MASK & ~(large_page_size - 1)) + (vaddr & (large_page_size - 1));
        return mapped == (paddr & PHYS_ADDR_MASK) ? 0 : -1;
    }

    page_table_entry *pt = next_table(physical_memory, pd, index2, flags);
    if(pt == NULL) return -1;

    pt[index1] = init_pt_entry(paddr, flags | PRESENT_BIT);
    return 0;
}

int map_large_page(pmm *physical_memory, page_map_level_4 pml4, uint64_t vaddr, uint64_t paddr, uint64_t flags)
{
    uint16_t index2 = (vaddr >> 21) & 0x1FF;
    uint16_t index3 = (vaddr >> 30) & 0x1FF;
    uint16_t index4 = (vaddr >> 39) & 0x1FF;

    page_directory_pointer_entry *pdp = next_table(physical_memory, pml4, index4, flags);
    if(pdp == NULL) return -1;

    page_directory_entry *pd = next_table(physical_memory, pdp, index3, flags);
    if(pd == NULL) return -1;

    /* Don't replace a page table: its 4 KiB mappings may differ. */
    if((pd[index2] & PRESENT_BIT) && !(pd[index2] & PAGE_SIZE_BIT)) return -1;

    pd[index2] = (paddr & PHYS_ADDR_MASK) | flags | PRESENT_BIT | PAGE_SIZE_BIT;
    return 0;
}

int map_range(pmm *physical_memory, page_map_level_4 pml4, uint64_t vaddr, uint64_t paddr, uint64_t length, uint64_t flags)
{
    uint64_t end = vaddr + length;
    vaddr &= ~(uint64_t)(page_size - 1);
    paddr &= ~(uint64_t)(page_size - 1);

    while(vaddr < end)
    {
        /* Use a 2 MiB page when both addresses are aligned, the range covers
           it, and no page table is already there. */
        if(!(vaddr & (large_page_size - 1)) && !(paddr & (large_page_size - 1)) &&
           end - vaddr >= large_page_size &&
           map_large_page(physical_memory, pml4, vaddr, paddr, flags) == 0)
        {
            vaddr += large_page_size;
            paddr += large_page_size;
            continue;
        }

        if(map_page(physical_memory, pml4, vaddr, paddr, flags) != 0) return -1;
        vaddr += page_size;
        paddr += page_size;
    }
    return 0;
}
