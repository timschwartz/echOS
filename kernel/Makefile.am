AUTOMAKE_OPTIONS = subdir-objects
bin_PROGRAMS = kernel
kernel_SOURCES = kernel.c memory/physical.c memory/paging.c memory/heap.c
kernel_CFLAGS = -ffreestanding -nostdlib
kernel_LDADD = ${KERNEL_LIBS} ${GCC_LIBS}
