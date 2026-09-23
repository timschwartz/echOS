#include "heap.h"
#include "physical.h"
#include "paging.h"

/* Back length bytes at vaddr with physically contiguous frames.
   Returns 0 on success and -1 on failure, in which case the frames are
   released (page tables created along the way are kept). */
int get_heap(pmm *physical_memory, page_map_level_4 PML4, uint64_t vaddr, size_t length)
{
    size_t pages = length / page_size;
    if((pages * page_size) < length) pages++;
    length = pages * page_size;

    uint64_t paddr = frames_allocate(physical_memory, pages);
    if(paddr == 0xFFFFFFFFFFFFFFFF) return -1;

    if(map_range(physical_memory, PML4, vaddr, paddr, length, READ_WRITE_BIT) != 0)
    {
        for(size_t i = 0; i < pages; i++) frame_free(physical_memory, paddr + i * page_size);
        return -1;
    }
    return 0;
}
