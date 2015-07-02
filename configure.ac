AC_INIT(src/bootsplash.c)

AC_PREREQ(2.52)

PACKAGE=GnublinBootSplash
VERSION=0.8.1

AC_SUBST(PACKAGE)
AC_SUBST(VERSION)

AM_INIT_AUTOMAKE($PACKAGE,$VERSION)


dnl Initialize maintainer mode
AM_MAINTAINER_MODE

ACLOCAL="$ACLOCAL $ACLOCAL_FLAGS"

AC_CANONICAL_HOST

AC_PROG_CC

dnl AC_SUBST(COMM_LIB,fifo.c)

dnl Find pkg-config
AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
if test "x$PKG_CONFIG" = "xno"; then
        AC_MSG_ERROR([You need to install pkg-config])
fi


dnl checking for getopt
AC_MSG_CHECKING(for getopt program)
GETOPT=getopt
AC_ARG_WITH(getopt, [  --with-getopt=GETOPT    name of the getopt program],
  GETOPT=$withval)
AC_MSG_RESULT($GETOPT)

AC_MSG_CHECKING(whether getopt handles long options)
opts=`$GETOPT --longoptions=help x -x`
if test x"$opts" != x" -x --"; then
   AC_MSG_ERROR([no

You need getopt from <http://huizen.dds.nl/~frodol/getopt.html>, or
else some other implementation that supports --longoptions.
])
fi
AC_MSG_RESULT(yes)


dnl Clear default CFLAGS
if test x"$CFLAGS" = x"-g -O2"; then
  CFLAGS=
fi
  
CFLAGS="-O3 -pipe $CFLAGS"

if test "x$GCC" = xyes; then
  CFLAGS="-Wall $CFLAGS"
fi

AC_ARG_ENABLE(debug, [  --enable-debug          turn on debugging [default=no]],, enable_debug=no)
if eval "test x$enable_debug = xyes"; then
  CFLAGS="$CFLAGS -g3"
fi



GLIB_REQUIRED_VERSION=2.0
PKG_CHECK_MODULES(GLIB, glib-2.0 >= $GLIB_REQUIRED_VERSION, has_glib=true,  AC_MSG_ERROR([unable to find the glib]))
AC_SUBST(GLIB_LIBS)
AC_SUBST(GLIB_CFLAGS)

#
# Check for DirectFB
#
PKG_CHECK_MODULES(DIRECTFB, directfb >= 0.9.20)


DATADIR=$datadir
AC_SUBST(DATADIR)

SYSCONFDIR=$sysconfdir
AC_SUBST(SYSCONFDIR)



AC_ARG_ENABLE(sdbusio, [  --enable-sdbusio          Turn on sd-bus communication with systemd instead of fifo for IPC for example with SYS-V style init systems [default=no]],enable_sdbusio=yes, enable_sdbusio=no)
AM_CONDITIONAL([SDBUS_IO], [test x$enable_sdbusio = xyes])
if eval "test x$enable_sdbusio = xyes"; then
    PKG_CHECK_MODULES(SYSTEMD, [libsystemd-login >= 32, libsystemd-daemon >= 32, libsystemd-journal >= 32], [have_systemd=yes], PKG_CHECK_MODULES([SYSTEMD], [libsystemd >= 209], [have_systemd=yes], [have_systemd=no]))
    if test "x$have_systemd" = "xno"; then
	AC_MSG_ERROR([
	    Tryied to compile with systemd support - but no suitable libsystemd found
	])
    fi

fi



# AC_SUBST(GLIB_DEPENDENCY_CFLAGS)
# AC_SUBST(GLIB_DEPENDENCY_LIBS)

CFLAGS="$CFLAGS -Werror-implicit-function-declaration -Wall -Werror"


AC_OUTPUT([
Makefile
data/Makefile
src/Makefile
])

