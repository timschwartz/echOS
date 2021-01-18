#!/bin/bash

export DEV_ROOT=/home/tim/projects/osdev
export SYSTEM_ROOT=${DEV_ROOT}/os
export INCLUDE_DIRS="-I /usr/include/efi /usr/include/efi/x86_64"
export CC=${DEV_ROOT}/cross/bin/x86_64-elf-gcc
export LD=${DEV_ROOT}/cross/bin/x86_64-elf-ld
export OBJCOPY=${DEV_ROOT}/cross/bin/x86_64-elf-objcopy
export LIB_DIRS="-L/usr/lib/gcc/x86_64-linux-gnu/10 -L/usr/lib"
export LOOP_TARGET=/mnt/loop
