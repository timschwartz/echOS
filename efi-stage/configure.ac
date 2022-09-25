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

AC_SUBST(CFLAGS, "$CFLAGS -I../include")
AC_SUBST(CC)

ifdef(LD, 
  AC_SUBST(LD),
  AC_SUBST(LD, "ld"))

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
