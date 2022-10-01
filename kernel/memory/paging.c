#include "paging.h"
#include "../kernel.h"
#include <string.h>

page_table_entry init_pt_entry(uint64_t paddr, uint64_t flags)
{
    page_table_entry entry = (page_table_entry)(paddr & 0xFFFFFFFFFFFFF000);
    entry |= flags;
    return entry;
}

page_table_entry *get_pt(page_directory_entry entry)
{
    return (page_table_entry *)(entry & 0xFFFFFFFFFFFFF000);
}

page_directory_entry init_pd_entry(pmm *physical_memory, uint64_t flags)
{
    uint64_t *pt = (uint64_t *)frame_allocate(physical_memory);
    memset(pt, 0, frame_size);
    page_directory_entry entry = (page_directory_entry)pt;
    entry |= flags;
    return entry;
}

page_directory_entry *get_pd(page_directory_pointer_entry entry)
{
    return (page_directory_entry *)(entry & 0xFFFFFFFFFFFFF000);
}

page_directory_pointer_entry init_pdp_entry(pmm *physical_memory, uint64_t flags)
{
    uint64_t *pd = (uint64_t *)frame_allocate(physical_memory);
    memset(pd, 0, frame_size);
    page_directory_pointer_entry entry = (page_directory_pointer_entry)pd;
    entry |= flags;
    return entry;
}

page_directory_pointer_entry *get_pdp(page_map_level_4_entry entry)
{
    return (page_directory_pointer_entry *)(entry & 0xFFFFFFFFFFFFF000);
}

page_map_level_4_entry init_pml4_entry(pmm *physical_memory, uint64_t flags)
{
    uint64_t *pdp = (uint64_t *)frame_allocate(physical_memory);
    memset(pdp, 0, frame_size);
    page_map_level_4_entry entry = (page_map_level_4_entry)pdp;
    entry |= flags;
    return entry;
}

page_map_level_4 init_pml4(pmm *physical_memory)
{
    page_map_level_4 pml4 = (page_map_level_4_entry *)frame_allocate(physical_memory);
    memset(pml4, 0, frame_size);
    return pml4;
}

uint64_t get_cr(size_t index)
{
    uint64_t cr[5];
    __asm__ __volatile__ (
        "mov %%cr0, %%rax\n\t"
        "mov %%eax, %0\n\t"
        "mov %%cr2, %%rax\n\t"
        "mov %%eax, %1\n\t"
        "mov %%cr3, %%rax\n\t"
        "mov %%eax, %2\n\t"
        "mov %%cr4, %%rax\n\t"
        "mov %%eax, %3\n\t"
    : "=m" (cr[0]), "=m" (cr[2]), "=m" (cr[3]), "=m" (cr[4])
    : /* no input */
    : "%rax"
    );

    return cr[index];
}

void set_cr3(uint64_t entry)
{
    __asm__ __volatile__ (
        "mov %%rax, %0\n\t"
        "mov %%cr3, %%rax\n\t"
        :
        : "m" (entry)
        : "memory"
    );
}

void *mmap(void *start, size_t length, int prot, int flags, 
           int fd, off_t offset)
{
    return (void *)0;
}

void map_page(page_map_level_4 pml4, uint64_t vaddr, uint64_t paddr, uint64_t flags)
{
    uint16_t index1 = (vaddr >> 12) & 0x1FF;
    uint16_t index2 = (vaddr >> 21) & 0x1FF;
    uint16_t index3 = (vaddr >> 30) & 0x1FF;
    uint16_t index4 = (vaddr >> 39) & 0x1FF;

    if(pml4[index4] == 0)
    {
        pml4[index4] = init_pml4_entry(system->physical_memory, flags);
    }

    page_directory_pointer_entry *pdp = get_pdp(pml4[index4]);

    if(pdp[index3] == 0)
    {
        pdp[index3] = init_pdp_entry(system->physical_memory, flags);
    }

    page_directory_entry *pd = get_pd(pdp[index3]);

    if(pd[index2] == 0)
    {
        pd[index2] = init_pd_entry(system->physical_memory, flags);
    }

    page_table_entry *pt = get_pt(pd[index2]);

    pt[index1] = init_pt_entry(paddr, flags);
}
