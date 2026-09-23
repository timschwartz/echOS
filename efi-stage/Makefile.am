AUTOMAKE_OPTIONS = subdir-objects
CLEANFILES = *.efi *.so
AM_CFLAGS = -c -fno-stack-protector -fpic -fshort-wchar -mno-red-zone -ffreestanding -nostdlib -DEFI_FUNCTION_WRAPPER -DGNU_EFI_USE_MS_ABI -I/usr/include/efi
EFI_LIBS = ${EFI_LIBDIR}/libgnuefi.a ${EFI_LIBDIR}/libefi.a
bin_PROGRAMS = bootx64.efi
bootx64_efi_SOURCES = main.c efi_mmap.c efi_malloc.c efi_pmm.c efi_video.c efi_fs.c

all-local: main.o efi_mmap.o efi_malloc.o efi_pmm.o efi_video.o efi_fs.o ../kernel/.libs/libkernel_static.a

	${LD} -o main.so main.o efi_mmap.o efi_malloc.o efi_pmm.o efi_video.o efi_fs.o \
		../kernel/.libs/libkernel_static.a \
		-nostdlib -znocombreloc -t -T ${EFI_LIBDIR}/elf_x86_64_efi.lds -shared -Bsymbolic \
		${EFI_LIBDIR}/crt0-efi-x86_64.o ${GCC_LIBS} ${EFI_LIBS} ${LIBC_LIBS}
	objcopy -j .text -j .sdata -j .data -j .rodata -j .dynamic -j .dynsym -j .rel -j .rela -j .reloc \
		-O efi-app-x86_64 main.so bootx64.efi
