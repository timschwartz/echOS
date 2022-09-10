AC_PREREQ([2.69])
AC_INIT([echOS], [0.0.2], [tim@metaverse.systems])
AC_CONFIG_SRCDIR([kernel/kernel.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIRS([m4])

AM_INIT_AUTOMAKE
LT_INIT
AM_PROG_AS
AC_PROG_CC

AC_CHECK_LIB([gcc], [main], 
  [HAVE_GCC=1] [GCC_LIBS="-L${ac_abs_confdir}/../cross/lib/gcc/x86_64-elf/10.2.0 -lgcc"], 
  AC_MSG_WARN([libgcc is not installed.]))
AC_SUBST(GCC_LIBS)

KERNEL_LIBS="-L${ac_abs_confdir}/lib/.libs -l:libk.a"
AC_SUBST(KERNEL_LIBS)

LIBC_LIBS="-L${ac_abs_confdir}/lib/.libs -l:libc.a"
AC_SUBST(LIBC_LIBS)

AC_SUBST(CFLAGS, "$CFLAGS -I../include")
AC_SUBST(CC)

ifdef(LD, 
  AC_SUBST(LD),
  AC_SUBST(LD, "ld"))

AC_CONFIG_FILES([Makefile
                 lib/Makefile
                 lib/string/Makefile
                 lib/stdlib/Makefile
                 lib/stdio/Makefile
                 kernel/Makefile])
AC_OUTPUT
