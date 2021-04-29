#include "efi_fs.h"

void *efi_malloc(UINTN poolSize);

uint8_t *efi_fread (CHAR16 *filename, size_t *length)
{
    EFI_STATUS result = -1;

    EFI_GUID sfspGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
    EFI_HANDLE* handles = NULL;
    UINTN handleCount = 0;
    result = uefi_call_wrapper(ST->BootServices->LocateHandleBuffer, 5, ByProtocol, &sfspGuid, NULL, &handleCount, &handles);
    if(EFI_ERROR(result))
    {
        Print(L"Couldn't get any handles\n");
        return NULL;
    }

    EFI_FILE_HANDLE file;

    for(int index = 0; index < (int)handleCount; ++index)
    {
        EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs = NULL;
        result = uefi_call_wrapper(ST->BootServices->HandleProtocol, 3, handles[index], &sfspGuid, (void **)&fs);
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

        result = uefi_call_wrapper(root->Open, 5, root, &file, filename, EFI_FILE_MODE_READ, 
                                   EFI_FILE_READ_ONLY | EFI_FILE_HIDDEN | EFI_FILE_SYSTEM);
        if(EFI_ERROR(result))
        {
            Print(L"Couldn't find file %s. Error: %d\n", filename, result);
        }
        else
        {
            break;
        }
    }

    if(EFI_ERROR(result)) for(;;);

    Print(L"Found %s.\n", filename);

    EFI_GUID FILE_INFO = EFI_FILE_INFO_ID;
    EFI_FILE_INFO *info;
    size_t info_length = sizeof(EFI_FILE_INFO) + 200;
    result = uefi_call_wrapper(file->GetInfo, 4, file, &FILE_INFO, &info_length, info);
    *length = info->FileSize;

    uint8_t *buffer = (uint8_t *)efi_malloc(*length);
    result = uefi_call_wrapper(file->Read, 3, file, (UINTN *)length, (void *)buffer);

    if(EFI_ERROR(result)) Print(L"FAILED\n");
    return buffer;
}
