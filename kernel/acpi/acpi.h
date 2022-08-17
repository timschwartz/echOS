#pragma once
#include <stdint.h>
#include <string.h>

typedef struct __attribute__((__packed__))
{
    char     Signature[8];
    uint8_t  Checksum;
    char     OEMID[6];
    uint8_t  Revision;
    uint32_t RsdtAddress;
} rsdp_desc;

typedef struct __attribute__((__packed__))
{
    rsdp_desc rsdp1;

    uint32_t Length;
    uint64_t XsdtAddress;
    uint8_t  ExtendedChecksum;
    uint8_t  reserved[3];
} rsdp2_desc;

typedef struct __attribute__((__packed__)) sdt_header
{
  char Signature[4];
  uint32_t Length;
  uint8_t Revision;
  uint8_t Checksum;
  char OEMID[6];
  char OEMTableID[8];
  uint32_t OEMRevision;
  uint32_t CreatorID;
  uint32_t CreatorRevision;
} sdt_header;

typedef struct __attribute__((__packed__))
{
    uint8_t EntryType;
    uint8_t RecordLength;
} madt_header;

typedef struct __attribute__((__packed__))
{
    madt_header header;
    uint8_t ProcessorID;
    uint8_t APICID;
    uint32_t flags;
} madt_processor;

typedef struct __attribute__((__packed__))
{
    madt_header header;
    uint8_t ID;
    uint8_t reserved;
    uint32_t Address;
    uint32_t InterruptBase;
} madt_ioapic;

typedef struct __attribute__((__packed__))
{
    sdt_header header;
    uint32_t LocalAPICAddress;
    uint32_t flags;
    madt_header entries[0];
} APIC;

typedef struct __attribute__((__packed__))
{
    sdt_header header;
} FADT;

typedef struct __attribute__((__packed__))
{
    sdt_header header;
    uint64_t ptr[0];
} XSDT;

size_t madt_get_processor_local_apic_count(APIC *apic);
void *xsdt_get_table(rsdp2_desc *rsdp, char signature[4]);
