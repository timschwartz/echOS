#include "heap.h"

void *get_heap(page_map_level_4 PML4, size_t length)
{
    size_t pages = length / page_size;
    if((pages * page_size) < length) pages++;
    length = pages * page_size;

    void *ptr = mmap(0, length, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);

    return ptr;
}
