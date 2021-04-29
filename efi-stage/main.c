#include <efi.h>
#include <efilib.h>
#include <string.h>
#include "efi_pmm.h"
#include "efi_video.h"
#include "efi_fs.h"

EFI_STATUS
EFIAPI
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);

    EFI_STATUS result = -1;

    uefi_call_wrapper(ST->ConOut->ClearScreen, 1, ST->ConOut);
    SystemTable->BootServices->SetWatchdogTimer(0, 0, 0, NULL);

    Print(L"EFI loader start.\n");

    draw_border(10, 0, 0x000070FF);

    setup_ssfn(L"EFI\\boot\\unifont.sfn");

//    uefi_call_wrapper((void *)SystemTable->BootServices->ExitBootServices, 2, ImageHandle, mmap.key);
    for(;;);

    return result;
}
