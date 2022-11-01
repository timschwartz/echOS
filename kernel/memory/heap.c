#include "heap.h"
#include "physical.h"
#include "paging.h"

void get_heap(pmm *physical_memory, page_map_level_4 PML4, uint64_t vaddr, size_t length)
{
    size_t pages = length / page_size;
    if((pages * page_size) < length) pages++;
    length = pages * page_size;

    uint64_t paddr = frames_allocate(physical_memory, pages);

    for(size_t i = 0; i < pages; i++)
    {
        uint64_t offset = i * page_size;
        map_page(physical_memory, PML4, vaddr + offset, paddr + offset, PRESENT_BIT | READ_WRITE_BIT);
    }
}
