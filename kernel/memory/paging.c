#include "paging.h"

page_table_entry init_pt_entry(pmm *physical_memory)
{
    uint64_t *page = (uint64_t *)frame_allocate(physical_memory);
    page_table_entry entry = (page_table_entry)page;
    entry |= PRESENT_BIT | READ_WRITE_BIT;
    return entry;
}

page_table_entry *get_pt(page_directory_entry entry)
{
    return (page_table_entry *)(entry & 0xFFFFFFFFFFFFF000);
}

page_directory_entry init_pd_entry(pmm *physical_memory)
{
    uint64_t *pt = (uint64_t *)frame_allocate(physical_memory);
    page_directory_entry entry = (page_directory_entry)pt;
    entry |= PRESENT_BIT | READ_WRITE_BIT;
    for(size_t i = 0; i < 512; i++)
    {
        pt[i] = init_pt_entry(physical_memory);
    }
    return entry;
}

page_directory_entry *get_pd(page_directory_pointer_entry entry)
{
    return (page_directory_entry *)(entry & 0xFFFFFFFFFFFFF000);
}

page_directory_pointer_entry init_pdp_entry(pmm *physical_memory)
{
    uint64_t *pd = (uint64_t *)frame_allocate(physical_memory);
    page_directory_pointer_entry entry = (page_directory_pointer_entry)pd;
    entry |= PRESENT_BIT | READ_WRITE_BIT;
    pd[0] = init_pd_entry(physical_memory);
    return entry;
}

page_directory_pointer_entry *get_pdp(page_map_level_4_entry entry)
{
    return (page_directory_pointer_entry *)(entry & 0xFFFFFFFFFFFFF000);
}

page_map_level_4_entry init_pml4_entry(pmm *physical_memory)
{
    uint64_t *pdp = (uint64_t *)frame_allocate(physical_memory);
    page_map_level_4_entry entry = (page_map_level_4_entry)pdp;
    entry |= PRESENT_BIT | READ_WRITE_BIT;
    pdp[0] = init_pdp_entry(physical_memory);
    return entry;
}

page_map_level_4 init_pml4(pmm *physical_memory)
{
    page_map_level_4 pml4 = (page_map_level_4_entry *)frame_allocate(physical_memory);
    pml4[0] = init_pml4_entry(physical_memory);
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

/*
void set_cr3(uint64_t entry)
{
    __asm__ __volatile__ (
        "
    );
}

    uint64_t cr0 = 0, cr2 = 0, cr3 = 0, cr4 = 0;
    __asm__ __volatile__ (
        "mov %%cr0, %%rax\n\t"
        "mov %%eax, %0\n\t"
        "mov %%cr2, %%rax\n\t"
        "mov %%eax, %1\n\t"
        "mov %%cr3, %%rax\n\t"
        "mov %%eax, %2\n\t"
    : "=m" (cr0), "=m" (cr2), "=m" (cr3)
    : // no input
    : "%rax"
    );
*/

void *mmap(void *start, size_t length, int prot, int flags, 
           int fd, off_t offset)
{
    return (void *)0;
}
