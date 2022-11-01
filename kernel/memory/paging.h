#pragma once
#include <stdint.h>
#include "physical.h"

#define page_size 4096

#define PRESENT_BIT 0x1
#define READ_WRITE_BIT 0x2
#define USER_SUPERVISOR_BIT 0x4
#define PAGE_LEVEL_WRITETHROUGH_BIT 0x8
#define PAGE_LEVEL_CACHE_DISABLE_BIT 0x10
#define ACCESSED_BIT 0x20
#define DIRTY_BIT 0x40
#define PAGE_SIZE_BIT 0x80
#define GLOBAL_PAGE_BIT 0x100
#define NX_BIT 0x8000000000000000

#define PROT_NONE       0
#define PROT_READ       1
#define PROT_WRITE      2
#define PROT_EXEC       4

#define MAP_FILE        0
#define MAP_SHARED      1
#define MAP_PRIVATE     2
#define MAP_TYPE        0xf
#define MAP_FIXED       0x10
#define MAP_ANONYMOUS   0x20
#define MAP_ANON        MAP_ANONYMOUS

#define MAP_FAILED      ((void *)-1)

/*

(47:39)      (38:30)
PML4[512] -> PDP[512]
          -> PDP[512]    (29:21)
          -> PDP[512] -> PD[512]    (20:12)
                      -> PD[512] -> PT[512]
*/

typedef uint64_t page_table_entry;
typedef uint64_t page_directory_entry;
typedef uint64_t page_directory_pointer_entry;
typedef uint64_t page_map_level_4_entry;
typedef page_map_level_4_entry *page_map_level_4;

page_directory_entry init_pd_entry(pmm *, uint64_t);
page_directory_pointer_entry init_pdp_entry(pmm *, uint64_t);
page_map_level_4_entry init_pml4_entry(pmm *, uint64_t);
page_map_level_4 init_pml4(pmm *);

page_directory_pointer_entry *get_pdp(page_map_level_4_entry entry);
page_directory_entry *get_pd(page_directory_pointer_entry entry);
page_table_entry *get_pt(page_directory_entry entry);
page_map_level_4 get_pml4(void);
uint64_t get_cr(size_t index);
void set_cr3(uint64_t entry);

void map_page(pmm *physical_memory, page_map_level_4 pml4, uint64_t vaddr, uint64_t paddr, uint64_t flags);

void dump_pte(page_table_entry entry);
void dump_pde(page_directory_entry entry);
void dump_pdpe(page_directory_pointer_entry entry);
