AUTOMAKE_OPTIONS = subdir-objects
SUBDIRS = string stdlib stdio

noinst_LTLIBRARIES = libc.la libk.la
AM_CFLAGS = -ffreestanding -nostdlib

libk_la_SOURCES = stack.c
libk_la_LIBADD = string/libstring.la stdlib/libstdlib.la stdio/libstdio.la

libc_la_SOURCES = crt0.s c.c stack.c
libc_la_LIBADD = string/libstring.la stdlib/libstdlib.la stdio/libstdio.la
