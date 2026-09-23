AC_PREREQ([2.69])
AC_INIT([echOS EFI stage], [0.0.2], [tim@metaverse.systems])
AC_CONFIG_SRCDIR([main.c])
AC_CONFIG_MACRO_DIRS([../m4])

AM_INIT_AUTOMAKE
LT_INIT
AM_PROG_AS
AC_PROG_CC

LIBC_LIBS="-L${ac_abs_confdir}/../lib/.libs -l:libc.a"
AC_SUBST(LIBC_LIBS)

AC_ARG_VAR([EFI_LIBDIR], [directory containing crt0-efi-x86_64.o, elf_x86_64_efi.lds and libgnuefi.a])
AC_MSG_CHECKING([for gnu-efi])
if test -z "$EFI_LIBDIR"; then
  for dir in /usr/lib/x86_64-linux-gnu /usr/lib /usr/lib64 /usr/local/lib; do
    if test -f "$dir/crt0-efi-x86_64.o"; then
      EFI_LIBDIR=$dir
      break
    fi
  done
fi
if test -z "$EFI_LIBDIR" || test ! -f "$EFI_LIBDIR/crt0-efi-x86_64.o"; then
  AC_MSG_ERROR([gnu-efi not found; install gnu-efi or set EFI_LIBDIR])
fi
AC_MSG_RESULT([$EFI_LIBDIR])

AC_SUBST(CFLAGS, "$CFLAGS -I../include")
AC_SUBST(CC)

ifdef(LD, 
  AC_SUBST(LD),
  AC_SUBST(LD, "ld"))

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
