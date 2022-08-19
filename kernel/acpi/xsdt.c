#include <stddef.h>
#include "acpi.h"

void *xsdt_get_table(rsdp2_desc *rsdp, char signature[4])
{
    XSDT *xsdt = (XSDT *)(rsdp->XsdtAddress);
    uint64_t tableCount = (xsdt->header.Length - sizeof(xsdt->header)) / 8;
    sdt_header *header = NULL;
    for(uint64_t i = 0; i < tableCount; i++)
    {
        header = (sdt_header *)(xsdt->ptr[i]);
        if(memcmp(header->Signature, signature, 4)) continue;
        return header;
    }
    return NULL;
}
