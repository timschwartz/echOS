#!/bin/bash

export SYSTEM_ROOT=${DEV_ROOT}/echOS
export INCLUDE_DIRS="-I /usr/include/efi -I /usr/include/efi/x86_64 -I ${SYSTEM_ROOT}/include"
export CFLAGS=${INCLUDE_DIRS}
export CC=${DEV_ROOT}/cross/bin/x86_64-elf-gcc
export LD=${DEV_ROOT}/cross/bin/x86_64-elf-ld
export OBJCOPY=${DEV_ROOT}/cross/bin/x86_64-elf-objcopy
export LIB_DIRS="-L${DEV_ROOT}/cross/lib/gcc/x86_64-elf/10.2.0"
export LOOP_TARGET=/mnt/loop
export CONFIG_SITE=${SYSTEM_ROOT}/config.echOS
