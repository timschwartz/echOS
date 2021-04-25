#include <efi.h>
#include <efilib.h>

void *efi_malloc(UINTN poolSize)
{
    EFI_STATUS status;
    void *handle;
    status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, poolSize, &handle);

    if(status == EFI_OUT_OF_RESOURCES)
    {
        Print(L"out of resources for pool\n");
        return 0;
    }
    else if(status == EFI_INVALID_PARAMETER)
    {
        Print(L"invalid pool type\n");
        return 0;
    }
    else
    {
        return handle;
    }
}
