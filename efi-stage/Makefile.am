AUTOMAKE_OPTIONS = subdir-objects
CLEANFILES = *.efi *.so
AM_CFLAGS = -c -fno-stack-protector -fpic -fshort-wchar -mno-red-zone -ffreestanding -DEFI_FUNCTION_WRAPPER -I/usr/include/efi
EFI_LIBS = -L/usr/lib -l:libgnuefi.a -l:libefi.a
bin_PROGRAMS = bootx64.efi
bootx64_efi_SOURCES = main.c efi_mmap.c efi_malloc.c efi_pmm.c efi_video.c efi_fs.c efi_random.c \
		../kernel/memory/physical.c \
		../kernel/memory/paging.c \
		../kernel/memory/heap.c \
		../kernel/acpi/xsdt.c

all-local: main.o efi_mmap.o efi_malloc.o efi_pmm.o efi_video.o efi_fs.o efi_random.o \
	../kernel/memory/physical.o ../kernel/memory/paging.o ../kernel/memory/heap.o ../kernel/acpi/xsdt.o

	${LD} -o main.so main.o efi_mmap.o efi_malloc.o efi_pmm.o efi_video.o efi_fs.o efi_random.o \
		 ../kernel/memory/physical.o ../kernel/memory/paging.o ../kernel/memory/heap.o \
		../kernel/acpi/xsdt.o \
		-nostdlib -znocombreloc -t -T /usr/lib/elf_x86_64_efi.lds -shared -Bsymbolic \
		/usr/lib/crt0-efi-x86_64.o ${EFI_STAGE_LIBS} ${GCC_LIBS} ${EFI_LIBS} ${LIBC_LIBS}
	objcopy -j .text -j .sdata -j .data -j .dynamic -j .dynsym -j .rel -j .rela -j .reloc \
		--target=efi-app-x86_64 main.so bootx64.efi
