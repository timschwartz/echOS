#pragma once
#include <stdint.h>
#include "paging.h"

void *get_heap(page_map_level_4 PML4, size_t length);
