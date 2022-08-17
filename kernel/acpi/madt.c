#include "acpi.h"

size_t madt_get_processor_local_apic_count(APIC *apic)
{
    size_t count = 0;

    size_t entries_size = apic->header.Length - sizeof(APIC);
    uint64_t offset = (uint64_t)&apic->entries;
    size_t pos = 0;
    madt_header *entry;
    while(pos < entries_size)
    {
        entry = (madt_header *)offset;
        switch(entry->EntryType)
        {
            case 0:
                count++;
                break;
            default:
                break;
        }
        pos += entry->RecordLength;
        offset += entry->RecordLength;
    }

    return count;
}
