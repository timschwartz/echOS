#include "physical.h"
#include <stddef.h>

uint64_t frame_allocate_from_block(pm_block *block)
{
    for(uint64_t entry = 0; entry < frame_map_size(block->frames_total); entry++)
    {
        if(block->map[entry] == 0xFFFFFFFFFFFFFFFF) continue;

        for(uint8_t bit = 0; bit < 64; bit++)
        {
            if(block->map[entry] & (1ULL << bit)) continue;

            block->map[entry] |= (1ULL << bit); 
            uint64_t frame_offset = (entry * 64) + bit;
            uint64_t address = block->address + (frame_offset * frame_size);
            block->frames_free--;
            return address;
        }
    }
    return 0xFFFFFFFFFFFFFFFF;
}

void frame_free_from_block(pm_block *block, uint64_t address)
{
    if(block->map == NULL) return;   // unmanaged region

    uint64_t offset = address - block->address;
    uint64_t frame_offset = offset / frame_size;
    uint64_t entry = frame_offset / 64;
    uint64_t bit = frame_offset % 64;

    if(block->map[entry] & (1ULL << bit)) block->frames_free++;
    block->map[entry] &= ~(1ULL << bit);
}

uint64_t frame_allocate(pmm *physical_memory)
{
    for(size_t i = 0; i < physical_memory->block_count; i++)
    {
        if(!physical_memory->blocks[i]->frames_free) continue;

        uint64_t address = frame_allocate_from_block(physical_memory->blocks[i]);
        if(address != 0xFFFFFFFFFFFFFFFF) return address;
    }
    return 0xFFFFFFFFFFFFFFFF;
}

void frame_free(pmm *physical_memory, uint64_t address)
{
    for(size_t i = 0; i < physical_memory->block_count; i++)
    {
        uint64_t end = physical_memory->blocks[i]->address +
                       (physical_memory->blocks[i]->frames_total * frame_size);

        if((address >= physical_memory->blocks[i]->address) &&
           (address < end))
        {
            frame_free_from_block(physical_memory->blocks[i], address);
            return;
        }
    }
}

static int frame_is_used(pm_block *block, uint64_t frame)
{
    return (block->map[frame / 64] >> (frame % 64)) & 1;
}

/* Find and claim count contiguous free frames in one block. */
static uint64_t frames_allocate_from_block(pm_block *block, size_t count)
{
    uint64_t run_start = 0;
    size_t run_length = 0;

    for(uint64_t frame = 0; frame < block->frames_total; frame++)
    {
        if(frame_is_used(block, frame))
        {
            run_length = 0;
            continue;
        }

        if(run_length == 0) run_start = frame;
        if(++run_length < count) continue;

        for(uint64_t f = run_start; f < run_start + count; f++)
        {
            block->map[f / 64] |= 1ULL << (f % 64);
        }
        block->frames_free -= count;
        return block->address + run_start * frame_size;
    }
    return 0xFFFFFFFFFFFFFFFF;
}

/* Allocate count physically contiguous frames and return the first one. */
uint64_t frames_allocate(pmm *physical_memory, size_t count)
{
    if(count == 0) return 0xFFFFFFFFFFFFFFFF;

    for(size_t i = 0; i < physical_memory->block_count; i++)
    {
        pm_block *block = physical_memory->blocks[i];
        if(block->map == NULL || block->frames_free < count) continue;

        uint64_t address = frames_allocate_from_block(block, count);
        if(address != 0xFFFFFFFFFFFFFFFF) return address;
    }
    return 0xFFFFFFFFFFFFFFFF;
}

size_t get_pmm_size(pmm *physical_memory)
{
    size_t length = physical_memory->block_count * sizeof(pm_block);
  
    for(size_t i = 0; i < physical_memory->block_count; i++)
    {
        pm_block *block = physical_memory->blocks[i];
        size_t size = frame_map_size(block->frames_total) * 8;
        length += size;
    }
    return length;
}
