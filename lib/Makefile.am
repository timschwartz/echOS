AUTOMAKE_OPTIONS = subdir-objects

noinst_LIBRARIES = libc.a libk.a libefi_stage.a
AM_CFLAGS = -ffreestanding -nostdlib

libk_a_SOURCES = stack.c string/memset.c

libefi_stage_a_SOURCES = stack.c

libc_a_SOURCES = crt0.s c.c stack.c string/memset.c stdlib/exit.c
