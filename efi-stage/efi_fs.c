#include "efi_fs.h"

void *efi_malloc(UINTN poolSize);

EFI_STATUS EFIAPI 
efi_fopen (EFI_SYSTEM_TABLE *SystemTable, EFI_FILE *file, CHAR16 *filename)
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

    return result;
}

EFI_STATUS EFIAPI
efi_fread(EFI_FILE *file, uint8_t *buffer)
{
    EFI_STATUS result;
    EFI_GUID FILE_INFO = EFI_FILE_INFO_ID;
    EFI_FILE_INFO *info;
    size_t length;
Print(L"  Calling GetInfo()\n");
    result = uefi_call_wrapper(file->GetInfo, 4, file, &FILE_INFO, &length, info);
    Print(L"Result: %d\n", result);
    return result;
//    result = file->GetInfo(file, &FILE_INFO, &length, info);
//    length = info->FileSize;
Print(L"File length: %d bytes\n", length);
    buffer = (uint8_t *)efi_malloc(length);
    return uefi_call_wrapper(file->Read, 3, file, &length, (void *)&buffer);
}
