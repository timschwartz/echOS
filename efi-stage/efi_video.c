#include "efi_video.h"

EFI_GRAPHICS_OUTPUT_PROTOCOL *get_gop(void)
{
    EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
 
    EFI_STATUS status = uefi_call_wrapper(BS->LocateProtocol, 3, &gopGuid, NULL, (void**)&gop);
    if(EFI_ERROR(status))
    {
        Print(L"Unable to locate Graphics Output Protocol\n");
        return NULL;
    }

    uefi_call_wrapper(gop->SetMode, 2, gop, 9);

    Print(L"Found Graphics Output Protocol\n");

    return gop;
}

EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *get_graphics_info(void)
{
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = get_gop();
    EFI_STATUS result;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
    UINTN info_len;
 
    Print(L"MaxMode: %d, Mode: %d\n", gop->Mode->MaxMode, gop->Mode->Mode);
    for(size_t i = 0; i < gop->Mode->MaxMode; i++)
    {
        result = uefi_call_wrapper(gop->QueryMode, 4, gop, i, &info_len, &info);
        if(EFI_ERROR(result) && result == EFI_NOT_STARTED)
        {
            uefi_call_wrapper(gop->SetMode, 2, gop, gop->Mode->Mode);
            result = uefi_call_wrapper(gop->QueryMode, 4, gop, i, &info_len, &info);
        }

        if(EFI_ERROR(result))
        {
            Print(L"ERROR: Bad response from QueryMode(%d). EFI_ERROR: %d\n", i, result);
            continue;
        }

        if(i != gop->Mode->Mode) continue;

        Print(L"%d: Width %d, height %d, format %x\n", i, info->HorizontalResolution, info->VerticalResolution, info->PixelFormat);
    }

    for(size_t x = 0; x < 1280; x++)
    {
        for(size_t y = 0; y < 10; y++)
        {
            *((uint32_t*)(gop->Mode->FrameBufferBase + 4 * gop->Mode->Info->PixelsPerScanLine * y + 4 * x)) = 0x000070FF;
        }
    }
}
