#pragma once
#include <stdint.h>
#include <efi.h>
#include <efilib.h>

void srand(uint32_t seed);
void efi_seed();
uint32_t rand();
