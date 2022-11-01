#pragma once
#include <stdint.h>
#include "paging.h"
#include "physical.h"

void get_heap(pmm *physical_memory, page_map_level_4 PML4, uint64_t vaddr, size_t length);
