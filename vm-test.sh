#!/bin/bash

source envvars.sh

cp efi-stage/bootx64.efi ${LOOP_TARGET}/EFI/boot/
qemu-system-x86_64 -m 8192 -cpu qemu64 -bios /usr/share/qemu/OVMF.fd -drive file=${DEV_ROOT}/images/disk.img,if=ide,format=raw 
