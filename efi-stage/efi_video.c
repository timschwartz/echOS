#include "efi_video.h"

EFI_GRAPHICS_OUTPUT_PROTOCOL *get_gop(void)
{
    EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
 
    EFI_STATUS status = uefi_call_wrapper(BS->LocateProtocol, 3, &gopGuid, NULL, (void**)&gop);
    if(EFI_ERROR(status))
    {
        Print(L"Unable to locate GOP\n");
        return NULL;
    }

    Print(L"Found GOP\n");
    return gop;
}
