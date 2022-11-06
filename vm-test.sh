#!/bin/bash

source envvars.sh

cd efi-stage
make
cd ..
cp efi-stage/bootx64.efi ${LOOP_TARGET}/EFI/boot/
cp efi-stage/*.sfn ${LOOP_TARGET}/EFI/boot/
cp kernel/.libs/libkernel.so.0.0.0 ${LOOP_TARGET}/EFI/boot/libkernel.so
# cp kernel/kernel ${LOOP_TARGET}/EFI/boot
qemu-system-x86_64 -s -M q35 -m 4096 -cpu qemu64 -smp 4 -bios /usr/share/qemu/OVMF.fd -drive file=${DEV_ROOT}/images/disk.img,if=virtio,format=raw
