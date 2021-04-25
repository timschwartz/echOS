AUTOMAKE_OPTIONS = subdir-objects
SUBDIRS = string stdlib

#noinst_LIBRARIES = libk.a libefi_stage.a
noinst_LTLIBRARIES = libc.la libk.la libefi_stage.la
AM_CFLAGS = -ffreestanding -nostdlib

libk_la_SOURCES = stack.c string/memset.c

libefi_stage_la_SOURCES = stack.c

libc_la_SOURCES = crt0.s c.c stack.c
libc_la_LIBADD = string/libstring.la stdlib/libstdlib.la
