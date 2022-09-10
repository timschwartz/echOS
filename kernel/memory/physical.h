#pragma once
#include <stdint.h>

#define frame_size 4096
#define frame_map_size(frame_count) (frame_count / 64)

typedef struct
{
    uint64_t address;
    uint64_t frames_total;
    uint64_t frames_free;
    uint64_t *map;
} pm_block;

typedef struct
{
    pm_block **blocks;
    size_t block_count;
} pmm;

uint64_t frame_allocate_from_block(pm_block *block);
void frame_free_from_block(pm_block *block, uint64_t address);

uint64_t frame_allocate(pmm *);
void frame_free(pmm *, uint64_t address);

uint64_t frames_allocate(pmm *, size_t);

size_t get_pmm_size(pmm *);
