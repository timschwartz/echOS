#include "paging.h"
#include "../kernel.h"
#include "../drivers/ssfn_fb.h"

void dump_pte(page_table_entry entry)
{
    ssfn_printf(system->fb, "Page table entry: 0x%llx\n", entry);

    ssfn_printf(system->fb, "    Physical address: 0x%llx\n", entry & 0xFFFFFFFFF000);
    ssfn_printf(system->fb, "    Present: %s\n", (entry & PRESENT_BIT) ? "True" : "False");
    ssfn_printf(system->fb, "    Writeable: %s\n", (entry & READ_WRITE_BIT) ? "True" : "False");
    ssfn_printf(system->fb, "    User accessible: %s\n", (entry & USER_SUPERVISOR_BIT) ? "False" : "True");
    ssfn_printf(system->fb, "    Page level write-through: %s\n", (entry & PAGE_LEVEL_WRITETHROUGH_BIT) ? "True" : "False");
    ssfn_printf(system->fb, "    Page level cache disable: %s\n", (entry & PAGE_LEVEL_CACHE_DISABLE_BIT) ? "True" : "False");
    ssfn_printf(system->fb, "    Accessed: %s\n", (entry & ACCESSED_BIT) ? "True" : "False");
    ssfn_printf(system->fb, "    Dirty: %s\n", (entry & DIRTY_BIT) ? "True" : "False");
    ssfn_printf(system->fb, "    PAT: %s\n", (entry & PAGE_SIZE_BIT) ? "1" : "0");
    ssfn_printf(system->fb, "    Global: %s\n", (entry & GLOBAL_PAGE_BIT) ? "True" : "False");
    ssfn_printf(system->fb, "    NX: %s\n", (entry & NX_BIT) ? "True" : "False");
}

void dump_paging_bits(uint64_t entry)
{
    ssfn_printf(system->fb, "    Physical address of table: 0x%llx\n", entry & 0xFFFFFFFFF000);
    ssfn_printf(system->fb, "    Present: %s\n", (entry & PRESENT_BIT) ? "True" : "False");
    ssfn_printf(system->fb, "    Writeable: %s\n", (entry & READ_WRITE_BIT) ? "True" : "False");
    ssfn_printf(system->fb, "    User accessible: %s\n", (entry & USER_SUPERVISOR_BIT) ? "False" : "True");
    ssfn_printf(system->fb, "    Page level write-through: %s\n", (entry & PAGE_LEVEL_WRITETHROUGH_BIT) ? "True" : "False");
    ssfn_printf(system->fb, "    Page level cache disable: %s\n", (entry & PAGE_LEVEL_CACHE_DISABLE_BIT) ? "True" : "False");
    ssfn_printf(system->fb, "    Accessed: %s\n", (entry & ACCESSED_BIT) ? "True" : "False");
    ssfn_printf(system->fb, "    Dirty: %s\n", (entry & DIRTY_BIT) ? "True" : "False");
    ssfn_printf(system->fb, "    Page size: %s\n", (entry & PAGE_SIZE_BIT) ? "1" : "0");
    ssfn_printf(system->fb, "    Global: %s\n", (entry & GLOBAL_PAGE_BIT) ? "True" : "False");
    ssfn_printf(system->fb, "    NX: %s\n", (entry & NX_BIT) ? "True" : "False");
}

void dump_pde(page_directory_entry entry)
{
    ssfn_printf(system->fb, "Page directory entry: 0x%llx\n", entry);
    dump_paging_bits(entry);
}

void dump_pdpe(page_directory_pointer_entry entry)
{
    ssfn_printf(system->fb, "Page directory pointer entry: 0x%llx\n", entry);
    dump_paging_bits(entry);
}
