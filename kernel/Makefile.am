AUTOMAKE_OPTIONS = subdir-objects

noinst_LTLIBRARIES = libkernel_static.la
lib_LTLIBRARIES = libkernel.la

libkernel_static_la_SOURCES = kernel.c memory/physical.c memory/paging.c memory/paging_debug.c memory/heap.c gdt.c idt.c drivers/ssfn_fb.c
libkernel_static_la_CFLAGS = -ffreestanding -nostdlib -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2
libkernel_static_la_LIBADD = ${KERNEL_LIBS} ${GCC_LIBS}

libkernel_la_SOURCES = kernel.c memory/physical.c memory/paging.c memory/paging_debug.c memory/heap.c gdt.c idt.c drivers/ssfn_fb.c
libkernel_la_CFLAGS = -ffreestanding -Wl,-Tlink.ld -nostdlib -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2
libkernel_la_LIBADD = ${KERNEL_LIBS} ${GCC_LIBS}
