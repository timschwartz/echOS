#include "efi_fs.h"
#include "efi_malloc.h"

/* Read a whole file from the first volume that has it into a new pool
   buffer. On success *buffer and *length describe the file; on failure
   nothing is allocated. */
EFI_STATUS efi_fread (CHAR16 *filename, size_t *length, uint8_t **buffer)
{
    EFI_STATUS result = -1;

    EFI_GUID sfspGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
    EFI_HANDLE* handles = NULL;
    UINTN handleCount = 0;
    result = uefi_call_wrapper(ST->BootServices->LocateHandleBuffer, 5, ByProtocol, &sfspGuid, NULL, &handleCount, &handles);
    if(EFI_ERROR(result)) return result;

    EFI_FILE_PROTOCOL *root = NULL;
    EFI_FILE_HANDLE file = NULL;
    result = EFI_NOT_FOUND;

    for(UINTN index = 0; index < handleCount; ++index)
    {
        EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs = NULL;
        if(EFI_ERROR(uefi_call_wrapper(ST->BootServices->HandleProtocol, 3, handles[index], &sfspGuid, (void **)&fs))) continue;
        if(EFI_ERROR(uefi_call_wrapper(fs->OpenVolume, 2, fs, &root))) continue;

        result = uefi_call_wrapper(root->Open, 5, root, &file, filename, EFI_FILE_MODE_READ,
                                   EFI_FILE_READ_ONLY | EFI_FILE_HIDDEN | EFI_FILE_SYSTEM);
        if(!EFI_ERROR(result)) break;

        uefi_call_wrapper(root->Close, 1, root);
        root = NULL;
        file = NULL;
    }
    uefi_call_wrapper(ST->BootServices->FreePool, 1, handles);

    if(file == NULL) return EFI_ERROR(result) ? result : EFI_NOT_FOUND;

    EFI_GUID FILE_INFO = EFI_FILE_INFO_ID;
    UINT64 info_buffer[(sizeof(EFI_FILE_INFO) + 512) / sizeof(UINT64)];
    EFI_FILE_INFO *info = (EFI_FILE_INFO *)info_buffer;
    UINTN info_length = sizeof(info_buffer);
    result = uefi_call_wrapper(file->GetInfo, 4, file, &FILE_INFO, &info_length, info);
    if(EFI_ERROR(result)) goto close;

    UINTN size = info->FileSize;
    uint8_t *data = (uint8_t *)efi_malloc(size);
    if(data == NULL)
    {
        result = EFI_OUT_OF_RESOURCES;
        goto close;
    }

    UINTN read = size;
    result = uefi_call_wrapper(file->Read, 3, file, &read, (void *)data);
    if(!EFI_ERROR(result) && read != size) result = EFI_END_OF_FILE;
    if(EFI_ERROR(result))
    {
        efi_free(data);
        goto close;
    }

    *buffer = data;
    *length = size;

close:
    uefi_call_wrapper(file->Close, 1, file);
    uefi_call_wrapper(root->Close, 1, root);
    return result;
}
