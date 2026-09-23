#include <efi.h>
#include <efilib.h>
#include "efi_malloc.h"

void *efi_malloc(uint64_t poolSize)
{
    void *handle = NULL;
    EFI_STATUS status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, poolSize, &handle);

    if(status == EFI_OUT_OF_RESOURCES)
    {
        Print(L"out of resources for pool\n");
        return NULL;
    }
    else if(status == EFI_INVALID_PARAMETER)
    {
        Print(L"invalid pool type\n");
        return NULL;
    }
    else if(EFI_ERROR(status))
    {
        Print(L"AllocatePool failed: 0x%lx\n", status);
        return NULL;
    }
    return handle;
}

void efi_free(void *handle)
{
    EFI_STATUS status = uefi_call_wrapper(BS->FreePool, 1, handle);
}
