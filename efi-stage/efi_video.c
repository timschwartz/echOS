#include "efi_video.h"
#include "efi_fs.h"
#define SSFN_CONSOLEBITMAP_TRUECOLOR
#include "ssfn.h"
#include <string.h>

uint16_t ssfn_margin = 0;

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

void ssfn_setup(CHAR16 *filename, uint16_t margin)
{
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = get_gop();

    size_t screen_width = gop->Mode->Info->HorizontalResolution;
    size_t screen_height = gop->Mode->Info->VerticalResolution;

    size_t length;
    uint8_t *free_sans_sfn = efi_fread(filename, &length);

    ssfn_src = (ssfn_font_t *)free_sans_sfn;
    ssfn_dst.ptr = (uint8_t *)gop->Mode->FrameBufferBase;
    ssfn_dst.p = 4 * gop->Mode->Info->PixelsPerScanLine;
    ssfn_dst.x = margin;
    ssfn_dst.y = margin;

    ssfn_set_color(0xFFFFFFFF, 0x0);

    ssfn_margin = margin;
}

void ssfn_set_color(uint32_t foreground, uint32_t background)
{
    ssfn_dst.fg = foreground;
    ssfn_dst.bg = background;
}

void ssfn_put(char c, size_t screen_width, size_t screen_height)
{
    ssfn_putc(c);
    if(ssfn_dst.x > screen_width) ssfn_putc('\n');
    if(ssfn_dst.x < ssfn_margin) ssfn_dst.x = ssfn_margin;
    if(ssfn_dst.y < ssfn_margin) ssfn_dst.y = ssfn_margin;
}

int ssfn_printf(char *format, ...)
{
    va_list values;
    va_start(values, format);

    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = get_gop();

    size_t screen_width = gop->Mode->Info->HorizontalResolution - ssfn_margin - 5;
    size_t screen_height = gop->Mode->Info->VerticalResolution;

    char temp[32] = {0};
    int written = 0;
    for(size_t i = 0; i < strlen(format); i++)
    {
        if(format[i] == '%')
        {
            i++;
            switch(format[i])
            {
                case 'd':
                    i++;
                    int d_value = va_arg(values, int);
                    itoa(d_value, temp, 10);
                    for(size_t counter = 0; counter < strlen(temp); counter++)
                    {
                        ssfn_put(temp[counter], screen_width, screen_height);
                        written++;
                    }
                    break;
                case 's':
                    i++;
                    char *s_value = va_arg(values, char *);
                    for(size_t counter = 0; counter < strlen(s_value); counter++)
                    {
                        ssfn_put(s_value[counter], screen_width, screen_height);
                        written++;
                    }
                    break;
            }
        }

        ssfn_put(format[i], screen_width, screen_height);
        written++;
    }
    return written;
}
