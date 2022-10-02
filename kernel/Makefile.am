AUTOMAKE_OPTIONS = subdir-objects
noinst_LTLIBRARIES = libkernel.la
libkernel_la_SOURCES = kernel.c memory/physical.c memory/paging.c memory/paging_debug.c memory/heap.c gdt.c idt.c drivers/ssfn_fb.c
libkernel_la_CFLAGS = -ffreestanding -nostdlib
libkernel_la_LIBADD = ${KERNEL_LIBS} ${GCC_LIBS}
