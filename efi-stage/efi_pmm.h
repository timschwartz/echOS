#pragma once
#include <efi.h>
#include <efilib.h>
#include "../kernel/memory/physical.h"

pmm *init_pmm(EFI_SYSTEM_TABLE *SystemTable);
