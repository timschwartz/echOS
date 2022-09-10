AUTOMAKE_OPTIONS = subdir-objects
bin_PROGRAMS = kernel
kernel_SOURCES = kernel.c memory/physical.c memory/paging.c memory/heap.c gdt.c idt.c drivers/ssfn_fb.c
kernel_CFLAGS = -ffreestanding -nostdlib -mgeneral-regs-only -mno-red-zone -pie -mcmodel=large -Wl,--emit-relocs
kernel_LDADD = ${KERNEL_LIBS} ${GCC_LIBS}

noinst_LTLIBRARIES = libkernel.la
libkernel_la_SOURCES = kernel.c memory/physical.c memory/paging.c memory/heap.c gdt.c idt.c drivers/ssfn_fb.c
libkernel_la_CFLAGS = -ffreestanding -nostdlib -mgeneral-regs-only -mno-red-zone -mcmodel=large
libkernel_la_LIBADD = ${KERNEL_LIBS} ${GCC_LIBS}
