CLEANFILES = *.efi *.so
AM_CFLAGS = -c -fno-stack-protector -fpic -fshort-wchar -mno-red-zone -ffreestanding -DEFI_FUNCTION_WRAPPER -I/usr/include/efi
EFI_LIBS = -L/usr/lib -l:libgnuefi.a -l:libefi.a
bin_PROGRAMS = bootx64.efi
bootx64_efi_SOURCES = main.c mmap.c

all-local: main.o mmap.o
	${LD} -o main.so main.o mmap.o -nostdlib -znocombreloc -t -T /usr/lib/elf_x86_64_efi.lds -shared -Bsymbolic \
		/usr/lib/crt0-efi-x86_64.o ${EFI_STAGE_LIBS} ${GCC_LIBS} ${EFI_LIBS}
	objcopy -j .text -j .sdata -j .data -j .dynamic -j .dynsym -j .rel -j .rela -j .reloc \
		--target=efi-app-x86_64 main.so bootx64.efi
