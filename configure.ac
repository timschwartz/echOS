AC_PREREQ([2.69])
AC_INIT([os], [0.0.1], [tim@metaverse.systems])
AC_CONFIG_SRCDIR([efi-stage/mmap.c])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE
AM_PROG_AS
AC_PROG_CC
AC_PROG_RANLIB

AC_CHECK_LIB([gcc], [main], 
  [HAVE_GCC=1] [GCC_LIBS="$GCC_LIBS -L/usr/lib/gcc/x86_64-linux-gnu/10 -lgcc"], 
  AC_MSG_WARN([libgcc is not installed.]))
AC_SUBST(GCC_LIBS)

KERNEL_LIBS="-L${ac_abs_confdir}/lib -l:libk.a"
AC_SUBST(KERNEL_LIBS)

EFI_STAGE_LIBS="-L${ac_abs_confdir}/lib -l:libefi_stage.a"
AC_SUBST(EFI_STAGE_LIBS)

AC_SUBST(CFLAGS, "$CFLAGS -I../include")
AC_SUBST(CC)

ifdef(LD, 
  AC_SUBST(LD),
  AC_SUBST(LD, "ld"))

AC_CONFIG_FILES([Makefile
                 efi-stage/Makefile
                 lib/Makefile
                 kernel/Makefile])
AC_OUTPUT
