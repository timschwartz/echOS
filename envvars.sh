#!/bin/bash

export SYSTEM_ROOT=${DEV_ROOT}/os
export INCLUDE_DIRS="-I /usr/include/efi /usr/include/efi/x86_64"
export CC=${DEV_ROOT}/cross/bin/x86_64-elf-gcc
export LD=${DEV_ROOT}/cross/bin/x86_64-elf-ld
export OBJCOPY=${DEV_ROOT}/cross/bin/x86_64-elf-objcopy
export LIB_DIRS="-L${DEV_ROOT}/cross/lib/gcc/x86_64-elf/10.2.0"
export LOOP_TARGET=/mnt/loop
