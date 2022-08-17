#include <efi.h>
#include <efilib.h>
#include <string.h>
#include "efi_pmm.h"
#include "efi_video.h"
#include "efi_fs.h"
#include "efi_random.h"
#include "../kernel/gdt.h"
#include "../kernel/acpi/acpi.h"
#include "../config.h"

const uint32_t cyan = 0x0000FFFF;
const uint32_t blue = 0x000070FF;
const uint32_t white = 0xFFFFFFFF;
const uint32_t black = 0x00000000;

EFI_GUID ACPI2 = ACPI_20_TABLE_GUID;

rsdp2_desc *get_rsdp2(EFI_SYSTEM_TABLE *SystemTable)
{
    for(uint16_t counter = 0; counter < SystemTable->NumberOfTableEntries; counter++)
    {
        EFI_CONFIGURATION_TABLE table = SystemTable->ConfigurationTable[counter];
        if(CompareGuid(&ACPI2, &table.VendorGuid)) continue;

        return (rsdp2_desc *)table.VendorTable;
    }
}

void dump_gdt()
{
    gdt_desc gdt = get_gdt();
    size_t count = (gdt.limit + 1) / sizeof(uint64_t);
    ssfn_printf("GDTR Base: 0x%x\n", gdt.base);
    ssfn_printf("GDTR Limit: 0x%x\n", gdt.limit);
    ssfn_printf("GDT has %d entries\n", count);

    for(uint16_t counter = 0; counter < count; counter++)
    {
        uint64_t *desc = (uint64_t *)(gdt.base + (counter * sizeof(uint64_t)));
        ssfn_printf("GDT %d: 0x%x\n", counter, *desc);
    }
}

void dump_sdt_header(sdt_header *sdt)
{
    char sig[5] = {0};
    memcpy(sig, sdt->Signature, 4);
    ssfn_printf("Signature: %s\n", sig);

    ssfn_printf("Length: %d\n", sdt->Length);

    char oemid[7] = {0};
    memcpy(oemid, sdt->OEMID, 6);
    ssfn_printf("OEMID: %s\n", oemid);

    char oemtable[5] = {0};
    memcpy(oemtable, sdt->OEMTableID, 4);
    ssfn_printf("OEMTableID: %s\n", oemtable);
}

void dump_apic_header(APIC *apic)
{
    dump_sdt_header(&(apic->header));

    ssfn_printf("Local APIC address: 0x%x\n", apic->LocalAPICAddress);
    ssfn_printf("Flags: 0x%x\n", apic->flags);
}

void dump_apic_entries(APIC *apic)
{
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
                madt_processor *cpu = (madt_processor *)entry;
                ssfn_printf("Found CPU. ID: %d. APIC ID: %d. ", cpu->ProcessorID, cpu->APICID);
                ssfn_printf("Enabled: %s. Online capable: %s.\n", cpu->flags & 0x1 ? "Yes" : "No", (cpu->flags >> 1) & 0x1 ? "Yes" : "No");
                break;
            case 1:
                madt_ioapic *ioapic = (madt_ioapic *)entry;
                ssfn_printf("Found IO APIC. ID: %d. IO APIC address: 0x%x. Interrupt base: 0x%x\n", ioapic->ID, ioapic->Address, ioapic->InterruptBase);
                break;
            default:
                ssfn_printf("Offset: 0x%x, Entry type: %d, length: %d bytes\n", offset, entry->EntryType, entry->RecordLength);
                break;
        }
        pos += entry->RecordLength;
        offset += entry->RecordLength;
    }
}

EFI_STATUS
EFIAPI
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);

    EFI_STATUS result = -1;

    uefi_call_wrapper(ST->ConOut->ClearScreen, 1, ST->ConOut);
    SystemTable->BootServices->SetWatchdogTimer(0, 0, 0, NULL);

    draw_border(10, 0, blue);

    ssfn_setup(L"\\EFI\\boot\\unifont.sfn", 20);
    ssfn_printf(PACKAGE_STRING);

    ssfn_set_color(0xFFFFFF00, 0xFFFF00FF);
    ssfn_printf(" EFI bootloader\n\n");

    ssfn_set_color(cyan, 0);

    rsdp2_desc *rsdp = get_rsdp2(SystemTable);

    APIC *apic = (APIC *)xsdt_get_table(rsdp, "APIC");

    ssfn_printf("Looking for CPUs\n");
    ssfn_printf("Found %d CPUs\n", madt_get_processor_local_apic_count(apic));

    dump_gdt();

//    uefi_call_wrapper((void *)SystemTable->BootServices->ExitBootServices, 2, ImageHandle, mmap.key);
    for(;;);

    return result;
}
