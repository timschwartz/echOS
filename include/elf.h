#pragma once

typedef struct
{
    char magic[4];
    uint8_t bits;
    uint8_t endian;
    uint8_t header_version;
    uint8_t os_abi;
    char pad1[8];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint64_t e_entry;
    uint64_t e_phoff;
    uint64_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;

} Elf64_Ehdr;
