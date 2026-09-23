#include "efi_pmm.h"
#include "efi_mmap.h"
#include "efi_malloc.h"

/* Extra pm_block slots for conventional-memory descriptors that appear
   between the provisional map and the final one (allocations split regions). */
#define PMM_BLOCK_SLACK 16

static int is_ram_type(UINT32 type)
{
    switch(type)
    {
        case EfiLoaderCode:
        case EfiLoaderData:
        case EfiBootServicesCode:
        case EfiBootServicesData:
        case EfiConventionalMemory:
            return 1;
        default:
            return 0;
    }
}

/* Size the PMM from a provisional memory map and allocate storage for it.
   Only RAM that the firmware could hand back as conventional memory counts
   towards the bitmap size, so MMIO windows don't inflate it. */
EFI_STATUS reserve_pmm(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable, pmm_storage *storage)
{
    efi_mmap_t mmap = { 0, 0, 0, 0, 0 };
    EFI_STATUS result = getEFIMemoryMap(ImageHandle, SystemTable, &mmap);
    if(result != EFI_SUCCESS)
    {
        Print(L"Error getting EFI memory map.\n");
        return result;
    }

    size_t block_count = 0;
    uint64_t ram_frames = 0;
    for(uint64_t offset = mmap.start; offset < mmap.end; offset += mmap.descriptorSize)
    {
        EFI_MEMORY_DESCRIPTOR *desc = (EFI_MEMORY_DESCRIPTOR *)offset;
        if(desc->Type == EfiConventionalMemory) block_count++;
        if(is_ram_type(desc->Type)) ram_frames += desc->NumberOfPages;
    }
    efi_free((void *)mmap.start);

    storage->block_capacity = block_count + PMM_BLOCK_SLACK;
    /* Each block wastes at most one partial word, hence + block_capacity. */
    storage->bitmap_words = ram_frames / 64 + storage->block_capacity;

    size_t bytes = sizeof(pmm)
                 + storage->block_capacity * sizeof(pm_block *)
                 + storage->block_capacity * sizeof(pm_block)
                 + storage->bitmap_words * sizeof(uint64_t);

    uint8_t *p = efi_malloc(bytes);
    if(p == NULL)
    {
        Print(L"Could not allocate %d bytes for the PMM.\n", bytes);
        return EFI_OUT_OF_RESOURCES;
    }

    storage->physical_memory = (pmm *)p;
    p += sizeof(pmm);
    storage->physical_memory->blocks = (pm_block **)p;
    p += storage->block_capacity * sizeof(pm_block *);
    for(size_t i = 0; i < storage->block_capacity; i++)
    {
        storage->physical_memory->blocks[i] = (pm_block *)p;
        p += sizeof(pm_block);
    }
    storage->bitmap = (uint64_t *)p;
    storage->physical_memory->block_count = 0;

    return EFI_SUCCESS;
}

/* Fill the PMM from the final memory map. Runs after ExitBootServices, so it
   must not allocate or Print. If the storage runs out, the remaining regions 
   are left unmanaged, which wastes memory but never hands out a frame that's
   in use. */
void build_pmm(pmm_storage *storage, efi_mmap_t mmap)
{
    pmm *physical_memory = storage->physical_memory;
    uint64_t *next_map = storage->bitmap;
    size_t words_left = storage->bitmap_words;

    physical_memory->block_count = 0;

    for(uint64_t offset = mmap.start; offset < mmap.end; offset += mmap.descriptorSize)
    {
        EFI_MEMORY_DESCRIPTOR *desc = (EFI_MEMORY_DESCRIPTOR *)offset;
        if(desc->NumberOfPages == 0) continue;
        if(physical_memory->block_count == storage->block_capacity) break;

        pm_block *block = physical_memory->blocks[physical_memory->block_count++];
        block->address = desc->PhysicalStart;
        block->frames_total = block->frames_free = desc->NumberOfPages;
        block->map = NULL;
        block->attributes = desc->Attribute;

        switch(desc->Type)
        {
            case EfiConventionalMemory:
                block->type = PM_USABLE;
                break;
            case EfiLoaderCode:
            case EfiLoaderData:
            case EfiBootServicesCode:
            case EfiBootServicesData:
                block->type = PM_RECLAIMABLE;
                break;
            case EfiACPIReclaimMemory:
                block->type = PM_ACPI_RECLAIM;
                break;
            case EfiACPIMemoryNVS:
                block->type = PM_ACPI_NVS;
                break;
            case EfiRuntimeServicesCode:
            case EfiRuntimeServicesData:
                block->type = PM_FIRMWARE;
                break;
            case EfiMemoryMappedIO:
            case EfiMemoryMappedIOPortSpace:
                block->type = PM_MMIO;
                break;
            default:
                block->type = PM_RESERVED;
                break;
        }

        if(block->type != PM_USABLE && block->type != PM_RECLAIMABLE) continue;

        size_t words = frame_map_size(desc->NumberOfPages);
        if(words > words_left) continue;
        block->map = next_map;
        next_map += words;
        words_left -= words;

        for(size_t j = 0; j < words; j++) block->map[j] = 0;
        /* Mark the bits past the end of the block in its last word as used. */
        if(desc->NumberOfPages % 64) block->map[words - 1] = ~0ULL << (desc->NumberOfPages % 64);
        /* Never hand out physical address 0: callers can't tell it from NULL. */
        if(block->address == 0)
        {
            block->map[0] |= 1;
            block->frames_free--;
        }
    }
}

void dump_pmm(pmm *pm)
{
    Print(L"Memory map at 0x%llx has %d blocks.\n", pm, pm->block_count);

    size_t total_frames = 0;
    for(size_t i = 0; i < pm->block_count; i++)
    {
        pm_block *b = pm->blocks[i];
        total_frames += b->frames_total;
        Print(L"Block %d: Address - 0x%llx, Phys. address - 0x%llx, total frames - %d, free frames - %d\n", i, b, b->address, b->frames_total, b->frames_free);
    }

    Print(L"Total frames: %d\n", total_frames);
}

void compare_pmm(pmm *a, pmm *b)
{
    Print(L"Comparing physical memory maps at a:0x%llx and b:0x%llx\n", a, b);
    if(a->block_count != b->block_count)
    {
        Print(L"Block count - a: %d, b: %d\n");
    }

    size_t block_count = a->block_count > b->block_count ? b->block_count : a->block_count;
    for(size_t i = 0; i < block_count; i++)
    {
        pm_block *a_block = a->blocks[i];
        pm_block *b_block = b->blocks[i];

        if(a_block->frames_total != b_block->frames_total)
        {
            Print(L"Block %d frames_total - a: %d, b: %d\n", i, a_block->frames_total, b_block->frames_total);
        }

        if(a_block->frames_free != b_block->frames_free)
        {
            Print(L"Block %d frames_free - a: %d, b: %d\n", i, a_block->frames_free, b_block->frames_free);
        }
    }
}
