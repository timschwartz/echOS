#include "physical.h"
#include <efi.h>
#include <efilib.h>

uint64_t frame_allocate_from_block(pm_block *block)
{
    for(uint64_t entry = 0; entry < frame_map_size(block->frames_total); entry++)
    {
        if(block->map[entry] == 0xFFFFFFFFFFFFFFFF) continue;

        for(uint8_t bit = 0; bit < 64; bit++)
        {
            if(block->map[entry] & (1 << bit)) continue;

            block->map[entry] |= (1 << bit); 
            uint64_t frame_offset = (entry * 8) + bit;
            uint64_t address = block->address + (frame_offset * frame_size);
            block->frames_free--;
            return address;
        }
    }
    return 0xFFFFFFFFFFFFFFFF;
}

void frame_free_from_block(pm_block *block, uint64_t address)
{
    uint64_t offset = address - block->address;
    uint64_t frame_offset = offset / frame_size;
    uint64_t entry = frame_offset / 64;
    uint64_t bit = frame_offset % 64;

    if(block->map[entry] & (1 << bit)) block->frames_free++;
    block->map[entry] &= ~(1 << bit);
}

uint64_t frame_allocate(pmm *physical_memory)
{
    for(size_t i = 0; i < physical_memory->block_count; i++)
    {
        if(!physical_memory->blocks[i]->frames_free) continue;

        return frame_allocate_from_block(physical_memory->blocks[i]);
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
           (address <= end))
        {
            frame_free_from_block(physical_memory->blocks[i], address);
            return;
        }
    }
}

uint64_t frames_allocate(pmm *physical_memory, size_t count)
{
    for(size_t i = 0; i < physical_memory->block_count; i++)
    {
//        if(i > 0) Print(L"Checking block %d. %d free frames\n", i, physical_memory->blocks[i]->frames_free);
        if(physical_memory->blocks[i]->frames_free < count) continue;

//        if(i > 0) Print(L"Choosing block %d\n", i);
        uint64_t address = frame_allocate_from_block(physical_memory->blocks[i]);
        for(size_t j = 1; j < count; j++) frame_allocate_from_block(physical_memory->blocks[i]);
        return address;
    }
//    Print(L"Couldn't find %d free frames\n", count);
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
