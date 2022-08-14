#!/bin/bash

source envvars.sh

cp efi-stage/bootx64.efi ${LOOP_TARGET}/EFI/boot/
cp efi-stage/*.sfn ${LOOP_TARGET}/EFI/boot/
cp kernel/kernel ${LOOP_TARGET}/EFI/boot
qemu-system-x86_64 -m 4096 -cpu qemu64 -smp 4 -bios /usr/share/qemu/OVMF.fd -drive file=${DEV_ROOT}/images/disk.img,if=ide,format=raw -rtc base=localtime
