#include "efi_video.h"
#include "efi_fs.h"
#define SSFN_IMPLEMENTATION
#include "ssfn.h"

ssfn_t ctx = { 0 };
ssfn_buf_t ssfn_buf;

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

    return gop;
}

EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *get_graphics_info(void)
{
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = get_gop();
    EFI_STATUS result;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
    UINTN info_len;
 
    for(size_t i = 0; i < gop->Mode->MaxMode; i++)
    {
        result = uefi_call_wrapper(gop->QueryMode, 4, gop, i, &info_len, &info);
        if(EFI_ERROR(result) && result == EFI_NOT_STARTED)
        {
            uefi_call_wrapper(gop->SetMode, 2, gop, 9);
            result = uefi_call_wrapper(gop->QueryMode, 4, gop, i, &info_len, &info);
        }

        if(EFI_ERROR(result))
        {
            Print(L"ERROR: Bad response from QueryMode(%d). EFI_ERROR: %d\n", i, result);
            continue;
        }

        if(i != gop->Mode->Mode) continue;
    }
    return info;
}

void draw_border(size_t width, size_t offset, uint32_t pixel)
{
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = get_gop();
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info = get_graphics_info();

    size_t screen_width = gop->Mode->Info->HorizontalResolution;
    size_t screen_height = gop->Mode->Info->VerticalResolution;

    for(size_t x = offset; x < screen_width - offset; x++)
    {
        for(size_t y = offset; y < width + offset; y++)
        {
            *((uint32_t*)(gop->Mode->FrameBufferBase + 4 * gop->Mode->Info->PixelsPerScanLine * y + 4 * x)) = pixel;
        }
    }

    for(size_t x = offset; x < screen_width - offset; x++)
    {
        for(size_t y = (screen_height - width - offset); y < screen_height - offset; y++)
        {
            *((uint32_t*)(gop->Mode->FrameBufferBase + 4 * gop->Mode->Info->PixelsPerScanLine * y + 4 * x)) = pixel;
        }
    }

    for(size_t y = offset; y < screen_height - offset; y++)
    {
        for(size_t x = offset; x < width + offset; x++)
        {
            *((uint32_t*)(gop->Mode->FrameBufferBase + 4 * gop->Mode->Info->PixelsPerScanLine * y + 4 * x)) = pixel;
        }
    }

    for(size_t y = offset; y < screen_height - offset; y++)
    {
        for(size_t x = screen_width - width - offset; x < screen_width - offset; x++)
        {
            *((uint32_t*)(gop->Mode->FrameBufferBase + 4 * gop->Mode->Info->PixelsPerScanLine * y + 4 * x)) = pixel;
        }
    }
}

void setup_ssfn(EFI_FILE *font)
{
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = get_gop();
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info = get_graphics_info();

    size_t screen_width = gop->Mode->Info->HorizontalResolution;
    size_t screen_height = gop->Mode->Info->VerticalResolution;

    ssfn_buf_t buf = {
        .ptr = (uint8_t *)gop->Mode->FrameBufferBase,
        .w = screen_width,
        .h = screen_height,
        .p = 4 * gop->Mode->Info->PixelsPerScanLine,
        .x = 100,
        .y = 100,
        .fg = 0xFF808080
    };

    ssfn_buf = buf;

    Print(L"Calling efi_fread\n");
    uint8_t *font_data;
    efi_fread(font, font_data);

    Print(L"%X %X\n", font_data[0], font_data[1]);
}
