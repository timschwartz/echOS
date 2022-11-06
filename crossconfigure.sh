#!/bin/bash

SYSTEM_ROOT=${DEV_ROOT}/echOS CFLAGS="-I /usr/include/efi -I /usr/include/efi/x86_64 -I ${SYSTEM_ROOT}/include" CC=${DEV_ROOT}/cross/bin/x86_64-elf-gcc LD=${DEV_ROOT}/cross/bin/x86_64-elf-ld OBJCOPY=${DEV_ROOT}/cross/bin/x86_64-elf-objcopy LIB_DIRS="-L${DEV_ROOT}/cross/lib/gcc/x86_64-elf/10.2.0" CONFIG_SITE=${SYSTEM_ROOT}/config.echOS ./configure
