#pragma once

void *efi_malloc(uint64_t poolSize);
void efi_free(void *handle);
