#include "efi_random.h"
#include "efi_video.h"

#define EFI_TIMESTAMP_PROTOCOL_GUID \
{ 0xafbfde41, 0x2e6e, 0x4262,\
 { 0xba, 0x65, 0x62, 0xb9, 0x23, 0x6e, 0x54, 0x95 }}

typedef UINT64 (EFIAPI *TIMESTAMP_GET) (VOID);
typedef struct {
    UINT64 Frequency;
    UINT64 EndValue;
} EFI_TIMESTAMP_PROPERTIES;
typedef EFI_STATUS (EFIAPI *TIMESTAMP_GET_PROPERTIES) (OUT EFI_TIMESTAMP_PROPERTIES *Properties);

typedef struct _EFI_TIMESTAMP_PROTOCOL {
    TIMESTAMP_GET GetTimestamp;
    TIMESTAMP_GET_PROPERTIES GetProperties;
} EFI_TIMESTAMP_PROTOCOL;

uint32_t next_random = 0;

void srand(uint32_t seed)
{
    next_random = seed;
}

void efi_seed()
{
    EFI_TIME time;
    RT->GetTime(&time, NULL);
    Print(L"Setting seed to %d\n", time.Nanosecond);
    srand(time.Nanosecond);
/*
    EFI_STATUS status = uefi_call_wrapper(ST->RuntimeServices->GetTime, 2, time, NULL);
    if(EFI_ERROR(status))
    {
        Print(L"Something happened.\n");
        return;
    }
*/

#if 0
    EFI_TIME *time;
    RT = ST->RuntimeServices;
    EFI_STATUS status = uefi_call_wrapper(RT->GetTime, 2, time, NULL);
    if(EFI_ERROR(status))
    {
        ssfn_set_color(0xFFFFFFFF, 0);
        ssfn_printf("Couldn't get time\n");
    }
    else
    {
        Print(L"setting seed to:");
        Print(L"%x\n", time->Nanosecond);
    }
    srand(time->Nanosecond);
#endif
}

uint32_t rand()
{
    if(next_random == 0) next_random = 0xCAFEBABE;
    int32_t hi = next_random / 127773;
    int32_t lo = next_random % 127773;
    int32_t temp = 16807 * lo - 2836 * hi;
    if(temp < 0) temp = -temp;
    next_random = temp;
    
    uint32_t value = temp % 0xFFFFFFFF;
    return value;
}
