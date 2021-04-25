#include <efi.h>
#include <efilib.h>
#include <string.h>
const uint64_t kernel_size = 5;

extern void kernel_start();
void (*kernel_entry)();

void *efi_malloc(UINTN poolSize);

EFI_STATUS
EFIAPI
efi_fopen (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable, CHAR16 *filename )
{
    EFI_STATUS result = -1;

    EFI_GUID sfspGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
    EFI_HANDLE* handles = NULL;
    UINTN handleCount = 0;
    result = uefi_call_wrapper(SystemTable->BootServices->LocateHandleBuffer, 5, ByProtocol, &sfspGuid, NULL, &handleCount, &handles);
    if(EFI_ERROR(result))
    {
        Print(L"Couldn't get any handles\n");
        return result;
    }

    for(int index = 0; index < (int)handleCount; ++index)
    {
        EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs = NULL;
        result = uefi_call_wrapper(SystemTable->BootServices->HandleProtocol, 3, handles[index], &sfspGuid, (void **)&fs);
        if(EFI_ERROR(result))
        {
            Print(L"HandleProtocol failed.\n");
        }

        EFI_FILE_PROTOCOL* root = NULL;
        result = uefi_call_wrapper(fs->OpenVolume, 2, fs, &root);
        if(EFI_ERROR(result))
        {
            Print(L"OpenVolume failed.\n");
        }

        EFI_FILE *kernel_image = NULL;

        result = uefi_call_wrapper(root->Open, 5, root, &kernel_image, filename, EFI_FILE_MODE_READ, 
                                   EFI_FILE_READ_ONLY | EFI_FILE_HIDDEN | EFI_FILE_SYSTEM);
        if(EFI_ERROR(result))
        {
            Print(L"Couldn't find ELF kernel.\n");
        }
        else
        {
            break;
        }
    }

    if(EFI_ERROR(result)) for(;;);

    Print(L"Found %s.\n", filename);

    return result;
}
