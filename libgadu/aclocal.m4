# aclocal.m4 generated automatically by aclocal 1.6.3 -*- Autoconf -*-

# Copyright 1996, 1997, 1998, 1999, 2000, 2001, 2002
# Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.

dnl Based on AC_NEED_STDINT_H by Guido Draheim <guidod@gmx.de> that can be
dnl found at http://www.gnu.org/software/ac-archive/. Do not complain him
dnl about this macro.
dnl
dnl $Id: aclocal.m4,v 1.17 2003/10/09 16:07:16 chilek Exp $

AC_DEFUN([AC_NEED_STDINT_H],
 [AC_MSG_CHECKING([for uintXX_t types])

  if test "x$1" = "x"; then
    ac_stdint_h="stdint.h"
  else
    ac_stdint_h="$1"
  fi

  rm -f $ac_stdint_h

  ac_header_stdint=""
  for i in stdint.h inttypes.h sys/inttypes.h sys/int_types.h sys/types.h; do
    if test "x$ac_header_stdint" = "x"; then
      AC_TRY_COMPILE([#include <$i>], [uint32_t foo], [ac_header_stdint=$i])
    fi
  done

  if test "x$ac_header_stdint" != "x" ; then
    AC_MSG_RESULT([found in <$ac_header_stdint>])
    STDINT_H="$ac_header_stdint"
    if test "x$ac_header_stdint" != "xstdint.h" ; then
      echo "#include <$ac_header_stdint>" > $ac_stdint_h
    fi
  else
    AC_MSG_RESULT([not found, using reasonable defaults])

    STDINT_H=""
    
    dnl let's make newer autoconf versions happy.
    stdint_h_foobar=define

    m4_pattern_allow([^__AC_STDINT_H$])
    
    cat > $ac_stdint_h << EOF
#ifndef __AC_STDINT_H
#$stdint_h_foobar __AC_STDINT_H 1

/* ISO C 9X: 7.18 Integer types <stdint.h> */

typedef unsigned char   uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

#ifndef __CYGWIN__
#define __int8_t_defined
typedef   signed char    int8_t;
typedef   signed short  int16_t;
typedef   signed int    int32_t;
#endif

#endif /* __AC_STDINT_H */
EOF
  fi
])

dnl Available from the GNU Autoconf Macro Archive at:
dnl http://www.gnu.org/software/ac-archive/htmldoc/acx_pthread.html
dnl
dnl Slightly modified by Wojtek Kaniewski <wojtekka@irc.pl> to remove
dnl dependency from AC_CANONICAL_HOST
dnl
AC_DEFUN([ACX_PTHREAD], [
AC_LANG_SAVE
AC_LANG_C
acx_pthread_ok=no

# We used to check for pthread.h first, but this fails if pthread.h
# requires special compiler flags (e.g. on True64 or Sequent).
# It gets checked for in the link test anyway.

# First of all, check if the user has set any of the PTHREAD_LIBS,
# etcetera environment variables, and if threads linking works using
# them:
if test x"$PTHREAD_LIBS$PTHREAD_CFLAGS" != x; then
        save_CFLAGS="$CFLAGS"
        CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
        save_LIBS="$LIBS"
        LIBS="$PTHREAD_LIBS $LIBS"
        AC_MSG_CHECKING([for pthread_join in LIBS=$PTHREAD_LIBS with CFLAGS=$PTHREAD_CFLAGS])
        AC_TRY_LINK_FUNC(pthread_join, acx_pthread_ok=yes)
        AC_MSG_RESULT($acx_pthread_ok)
        if test x"$acx_pthread_ok" = xno; then
                PTHREAD_LIBS=""
                PTHREAD_CFLAGS=""
        fi
        LIBS="$save_LIBS"
        CFLAGS="$save_CFLAGS"
fi

# We must check for the threads library under a number of different
# names; the ordering is very important because some systems
# (e.g. DEC) have both -lpthread and -lpthreads, where one of the
# libraries is broken (non-POSIX).

# Create a list of thread flags to try.  Items starting with a "-" are
# C compiler flags, and other items are library names, except for "none"
# which indicates that we try without any flags at all.

acx_pthread_flags="pthreads none -Kthread -kthread lthread -pthread -pthreads -mthreads pthread --thread-safe -mt"

# The ordering *is* (sometimes) important.  Some notes on the
# individual items follow:

# pthreads: AIX (must check this before -lpthread)
# none: in case threads are in libc; should be tried before -Kthread and
#       other compiler flags to prevent continual compiler warnings
# -Kthread: Sequent (threads in libc, but -Kthread needed for pthread.h)
# -kthread: FreeBSD kernel threads (preferred to -pthread since SMP-able)
# lthread: LinuxThreads port on FreeBSD (also preferred to -pthread)
# -pthread: Linux/gcc (kernel threads), BSD/gcc (userland threads)
# -pthreads: Solaris/gcc
# -mthreads: Mingw32/gcc, Lynx/gcc
# -mt: Sun Workshop C (may only link SunOS threads [-lthread], but it
#      doesn't hurt to check since this sometimes defines pthreads too;
#      also defines -D_REENTRANT)
# pthread: Linux, etcetera
# --thread-safe: KAI C++

UNAME_SYSTEM=`(uname -s) 2> /dev/null` || UNAME_SYSTEM=unknown

case "$UNAME_SYSTEM" in
        *SunOS*)

        # On Solaris (at least, for some versions), libc contains stubbed
        # (non-functional) versions of the pthreads routines, so link-based
        # tests will erroneously succeed.  (We need to link with -pthread or
        # -lpthread.)  (The stubs are missing pthread_cleanup_push, or rather
        # a function called by this macro, so we could check for that, but
        # who knows whether they'll stub that too in a future libc.)  So,
        # we'll just look for -pthreads and -lpthread first:

        acx_pthread_flags="-pthread -pthreads pthread -mt $acx_pthread_flags"
        ;;
esac

if test x"$acx_pthread_ok" = xno; then
for flag in $acx_pthread_flags; do

        case $flag in
                none)
                AC_MSG_CHECKING([whether pthreads work without any flags])
                ;;

                -*)
                AC_MSG_CHECKING([whether pthreads work with $flag])
                PTHREAD_CFLAGS="$flag"
                ;;

                *)
                AC_MSG_CHECKING([for the pthreads library -l$flag])
                PTHREAD_LIBS="-l$flag"
                ;;
        esac

        save_LIBS="$LIBS"
        save_CFLAGS="$CFLAGS"
        LIBS="$PTHREAD_LIBS $LIBS"
        CFLAGS="$CFLAGS $PTHREAD_CFLAGS"

        # Check for various functions.  We must include pthread.h,
        # since some functions may be macros.  (On the Sequent, we
        # need a special flag -Kthread to make this header compile.)
        # We check for pthread_join because it is in -lpthread on IRIX
        # while pthread_create is in libc.  We check for pthread_attr_init
        # due to DEC craziness with -lpthreads.  We check for
        # pthread_cleanup_push because it is one of the few pthread
        # functions on Solaris that doesn't have a non-functional libc stub.
        # We try pthread_create on general principles.
        AC_TRY_LINK([#include <pthread.h>],
                    [pthread_t th; pthread_join(th, 0);
                     pthread_attr_init(0); pthread_cleanup_push(0, 0);
                     pthread_create(0,0,0,0); pthread_cleanup_pop(0); ],
                    [acx_pthread_ok=yes])

        LIBS="$save_LIBS"
        CFLAGS="$save_CFLAGS"

        AC_MSG_RESULT($acx_pthread_ok)
        if test "x$acx_pthread_ok" = xyes; then
                break;
        fi

        PTHREAD_LIBS=""
        PTHREAD_CFLAGS=""
done
fi

# Various other checks:
if test "x$acx_pthread_ok" = xyes; then
        save_LIBS="$LIBS"
        LIBS="$PTHREAD_LIBS $LIBS"
        save_CFLAGS="$CFLAGS"
        CFLAGS="$CFLAGS $PTHREAD_CFLAGS"

        # Detect AIX lossage: threads are created detached by default
        # and the JOINABLE attribute has a nonstandard name (UNDETACHED).
        AC_MSG_CHECKING([for joinable pthread attribute])
        AC_TRY_LINK([#include <pthread.h>],
                    [int attr=PTHREAD_CREATE_JOINABLE;],
                    ok=PTHREAD_CREATE_JOINABLE, ok=unknown)
        if test x"$ok" = xunknown; then
                AC_TRY_LINK([#include <pthread.h>],
                            [int attr=PTHREAD_CREATE_UNDETACHED;],
                            ok=PTHREAD_CREATE_UNDETACHED, ok=unknown)
        fi
        if test x"$ok" != xPTHREAD_CREATE_JOINABLE; then
                AC_DEFINE(PTHREAD_CREATE_JOINABLE, $ok,
                          [Define to the necessary symbol if this constant
                           uses a non-standard name on your system.])
        fi
        AC_MSG_RESULT(${ok})
        if test x"$ok" = xunknown; then
                AC_MSG_WARN([we do not know how to create joinable pthreads])
        fi

        AC_MSG_CHECKING([if more special flags are required for pthreads])
        flag=no
	case "$UNAME_SYSTEM" in 
                *AIX* | *FreeBSD*)     flag="-D_THREAD_SAFE";;
                *SunOS* | *OSF* | *HP-UX*) flag="-D_REENTRANT";;
        esac
        AC_MSG_RESULT(${flag})
        if test "x$flag" != xno; then
                PTHREAD_CFLAGS="$flag $PTHREAD_CFLAGS"
        fi

        LIBS="$save_LIBS"
        CFLAGS="$save_CFLAGS"

        # More AIX lossage: must compile with cc_r
        AC_CHECK_PROG(PTHREAD_CC, cc_r, cc_r, ${CC})
else
        PTHREAD_CC="$CC"
fi

AC_SUBST(PTHREAD_LIBS)
AC_SUBST(PTHREAD_CFLAGS)
AC_SUBST(PTHREAD_CC)

# Finally, execute ACTION-IF-FOUND/ACTION-IF-NOT-FOUND:
if test x"$acx_pthread_ok" = xyes; then
        ifelse([$1],,AC_DEFINE(HAVE_PTHREAD,1,[Define if you have POSIX threads libraries and header files.]),[$1])
        :
else
        acx_pthread_ok=no
        $2
fi
AC_LANG_RESTORE
])dnl ACX_PTHREAD

dnl based on curses.m4 
dnl $Id: aclocal.m4,v 1.17 2003/10/09 16:07:16 chilek Exp $

AC_DEFUN(AC_CHECK_OPENSSL,[
  AC_SUBST(OPENSSL_LIBS)
  AC_SUBST(OPENSSL_INCLUDES)

  AC_ARG_WITH(openssl,
    [[  --without-openssl       Compile without OpenSSL]], 
      if test "x$withval" = "xno" ; then
        without_openssl=yes
      elif test "x$withval" != "xyes" ; then
        with_arg=$withval/include:-L$withval/lib
      fi)

  if test "x$without_openssl" != "xyes" -a "x$with_arg" = "x"; then
    AC_CHECK_PROG([PKGCONFIG], [pkg-config], [pkg-config], [no])
    if test "x$PKGCONFIG" != "xno"; then
      AC_MSG_CHECKING([for OpenSSL])
      OPENSSL_LIBS=$($PKGCONFIG --libs openssl)
      OPENSSL_INCLUDES=$($PKGCONFIG --cflags openssl)
      if test "x$OPENSSL_LIBS" != "x" -o "x$OPENSSL_INCLUDES" != "x"; then
	AC_DEFINE(HAVE_OPENSSL, 1, [define if you have OpenSSL])
	AC_MSG_RESULT([yes])
        without_openssl=yes
	have_openssl=yes
      else
        AC_MSG_RESULT([no])
      fi
    fi
  fi

  if test "x$without_openssl" != "xyes" ; then
    AC_MSG_CHECKING(for ssl.h)

    for i in $with_arg \
    		/usr/include: \
		/usr/local/include:"-L/usr/local/lib" \
		/usr/local/ssl/include:"-L/usr/local/ssl/lib" \
		/usr/pkg/include:"-L/usr/pkg/lib" \
		/usr/contrib/include:"-L/usr/contrib/lib" \
		/usr/freeware/include:"-L/usr/freeware/lib32" \
    		/sw/include:"-L/sw/lib" \
    		/cw/include:"-L/cw/lib" \
		/boot/home/config/include:"-L/boot/home/config/lib"; do
	
      incl=`echo "$i" | sed 's/:.*//'`
      lib=`echo "$i" | sed 's/.*://'`

      if test -f $incl/openssl/ssl.h; then
        AC_MSG_RESULT($incl/openssl/ssl.h)
	ldflags_old="$LDFLAGS"
	LDFLAGS="$lib -lssl -lcrypto"
	save_LIBS="$LIBS"
	LIBS="-lssl -lcrypto $LIBS"
	AC_CHECK_LIB(ssl, RSA_new, [
	  AC_DEFINE(HAVE_OPENSSL, 1, [define if you have OpenSSL])
	  have_openssl=yes
	  OPENSSL_LIBS="$lib -lssl -lcrypto"
	  if test "x$incl" != "x/usr/include"; then
    	    OPENSSL_INCLUDES="-I$incl"
	  fi
	])
	LIBS="$save_LIBS"
	LDFLAGS="$ldflags_old"
	break
      fi
    done

    if test "x$have_openssl" != "xyes"; then
      AC_MSG_RESULT(not found)
    fi
  fi
])

dnl Rewritten from scratch. --wojtekka
dnl $Id: aclocal.m4,v 1.17 2003/10/09 16:07:16 chilek Exp $

AC_DEFUN(AC_CHECK_NCURSES,
[
	AC_SUBST(CURSES_LIBS)
	AC_SUBST(CURSES_INCLUDES)

	AC_ARG_WITH(ncurses, [[  --with-ncurses[=dir]    Compile with ncurses/locate base dir]],
	[
		if test "x$withval" = "xno" ; then
			without_ncurses=yes
			with_arg=""
		elif test "x$withval" != "xyes" ; then
			with_arg=$withval/include:-L$withval/lib
		fi
	])

	if test "x$without_ncurses" != "xyes" ; then
		for i in $with_arg \
			: \
			-I/usr/pkg/include:-L/usr/pkg/lib \
			-I/usr/contrib/include:-L/usr/contrib/lib \
			-I/usr/freeware/include:-L/usr/freeware/lib32 \
			-I/sw/include:-L/sw/lib \
			-I/cw/include:-L/cw/lib \
			-I/boot/home/config/include:-L/boot/home/config/lib; do
	
			incl=`echo "$i" | sed 's/:.*//'`	# 'g³upi vim
			lib=`echo "$i" | sed 's/.*://'`		# 'g³upi vim
			path=`echo "$incl" | sed 's/^..//'`	# 'g³upi vim

			cppflags="$CPPFLAGS"
			ldflags="$LDFLAGS"

			CPPFLAGS="$CPPFLAGS $incl"
			LDFLAGS="$LDFLAGS $libs"

			have_ncurses_h=""

			if test "x$path" = "x" -o -f "$path/ncurses.h" -o -f "$path/ncurses/ncurses.h"; then
				$as_unset ac_cv_header_ncurses_h
				$as_unset ac_cv_header_ncurses_ncurses_h
				AC_CHECK_HEADERS([ncurses.h], [
					CURSES_INCLUDES="$incl"
					have_ncurses_h=yes
				], [
					AC_CHECK_HEADERS([ncurses/ncurses.h],
					[
						CURSES_INCLUDES="$incl"
						have_ncurses_h=yes
					])
				])
			fi

			if test "x$have_ncurses_h" = "xyes"; then
				$as_unset ac_cv_lib_ncurses_initscr
				$as_unset ac_cv_lib_curses_initscr
				have_libncurses=""
				AC_CHECK_LIB(ncurses, initscr,
				[
					CURSES_LIBS="$libs -lncurses"
					have_libncurses=yes
				], [
					AC_CHECK_LIB(curses, initscr,
					[
						CURSES_LIBS="$libs -lcurses"
						have_libncurses=yes
					])
				])

				if test "x$have_libncurses" = "xyes"; then
					AC_DEFINE(HAVE_NCURSES, 1, [define if ncurses is installed])
					have_ncurses=yes
					break
				fi
			fi

			CPPFLAGS="$cppflags"
			LDFLAGS="$ldflags"
		done
	fi
])



dnl Rewritten from scratch. --wojtekka
dnl $Id: aclocal.m4,v 1.17 2003/10/09 16:07:16 chilek Exp $

AC_DEFUN(AC_CHECK_READLINE,[
  AC_SUBST(READLINE_LIBS)
  AC_SUBST(READLINE_INCLUDES)

  AC_ARG_WITH(readline,
    [[  --with-readline[=dir]   Compile with readline/locate base dir]],
    if test "x$withval" = "xno" ; then
      without_readline=yes
    elif test "x$withval" != "xyes" ; then
      with_arg="$withval/include:-L$withval/lib $withval/include/readline:-L$withval/lib"
    fi)

  AC_MSG_CHECKING(for readline.h)

  if test "x$without_readline" != "xyes"; then
    for i in $with_arg \
	     /usr/include: \
	     /usr/local/include:-L/usr/local/lib \
             /usr/freeware/include:-L/usr/freeware/lib32 \
	     /usr/pkg/include:-L/usr/pkg/lib \
	     /sw/include:-L/sw/lib \
	     /cw/include:-L/cw/lib \
	     /net/caladium/usr/people/piotr.nba/temp/pkg/include:-L/net/caladium/usr/people/piotr.nba/temp/pkg/lib \
	     /boot/home/config/include:-L/boot/home/config/lib; do
    
      incl=`echo "$i" | sed 's/:.*//'`
      lib=`echo "$i" | sed 's/.*://'`

      if test -f $incl/readline/readline.h ; then
        AC_MSG_RESULT($incl/readline/readline.h)
        READLINE_LIBS="$lib -lreadline"
	if test "$incl" != "/usr/include"; then
	  READLINE_INCLUDES="-I$incl/readline -I$incl"
	else
	  READLINE_INCLUDES="-I$incl/readline"
	fi
        AC_DEFINE(HAVE_READLINE, 1, [define if You want readline])
        have_readline=yes
        break
      elif test -f $incl/readline.h -a "x$incl" != "x/usr/include"; then
        AC_MSG_RESULT($incl/readline.h)
        READLINE_LIBS="$lib -lreadline"
        READLINE_INCLUDES="-I$incl"
        AC_DEFINE(HAVE_READLINE, 1, [define if You want readline])
        have_readline=yes
        break
      fi
    done
  fi

  if test "x$have_readline" != "xyes"; then
    AC_MSG_RESULT(not found)
  fi
])


dnl Rewritten from scratch. --speedy 
dnl $Id: aclocal.m4,v 1.17 2003/10/09 16:07:16 chilek Exp $

PYTHON=
PYTHON_VERSION=
PYTHON_INCLUDES=
PYTHON_LIBS=

AC_DEFUN(AC_CHECK_PYTHON,[
  AC_SUBST(PYTHON_LIBS)
  AC_SUBST(PYTHON_INCLUDES)

  AC_ARG_WITH(python,
    [[  --with-python     	  Compile with Python bindings]],
    if test "x$withval" != "xno" -a "x$withval" != "xyes"; then
	with_arg="$withval/include:-L$withval/lib $withval/include/python:-L$withval/lib"
    fi
  )

	if test "x$with_python" = "xyes"; then			    
		
		AC_PATH_PROG(PYTHON, python)
		if test "$PYTHON" = ""; then AC_PATH_PROG(PYTHON, python2.2) fi
		if test "$PYTHON" = ""; then AC_PATH_PROG(PYTHON, python2.1) fi
		if test "$PYTHON" = ""; then AC_PATH_PROG(PYTHON, python2.0) fi
		
		if test "$PYTHON" != ""; then 
			PY_VERSION=`$PYTHON -c "import sys; print sys.version[[0:3]]"`
			PY_PREFIX=`$PYTHON -c "import sys; print sys.prefix"`
			echo "Found Python version $PY_VERSION [$PY_PREFIX]"
		fi

		AC_MSG_CHECKING(for Python.h)
		
		PY_EXEC_PREFIX=`$PYTHON -c "import sys; print sys.exec_prefix"`
		
		if test "$PY_VERSION" != ""; then 
		    if test -f $PY_PREFIX/include/python$PY_VERSION/Python.h ; then 
			AC_MSG_RESULT($PY_PREFIX/include/python$PY_VERSION/Python.h)
			PY_LIB_LOC="-L$PY_EXEC_PREFIX/lib/python$PY_VERSION/config"
			PY_CFLAGS="-I$PY_PREFIX/include/python$PY_VERSION"
			PY_MAKEFILE="$PY_EXEC_PREFIX/lib/python$PY_VERSION/config/Makefile"

			PY_LOCALMODLIBS=`sed -n -e 's/^LOCALMODLIBS=\(.*\)/\1/p' $PY_MAKEFILE`
			PY_BASEMODLIBS=`sed -n -e 's/^BASEMODLIBS=\(.*\)/\1/p' $PY_MAKEFILE`
			PY_OTHER_LIBS=`sed -n -e 's/^LIBS=\(.*\)/\1/p' $PY_MAKEFILE`
			PY_OTHER_LIBM=`sed -n -e 's/^LIBC=\(.*\)/\1/p' $PY_MAKEFILE`
			PY_OTHER_LIBC=`sed -n -e 's/^LIBM=\(.*\)/\1/p' $PY_MAKEFILE`
			PY_LINKFORSHARED="$(python -c "import distutils.sysconfig; print distutils.sysconfig.get_config_var('LINKFORSHARED')")"
			PY_LIBS="$PY_LOCALMODLIBS $PY_BASEMODLIBS $PY_OTHER_LIBS $PY_OTHER_LIBC $PY_OTHER_LIBM $PY_LINKFORSHARED"

			PYTHON_LIBS="-L$PY_EXEC_PREFIX/lib $PY_LIB_LOC -lpython$PY_VERSION $PY_LIBS"
			PYTHON_INCLUDES="$PY_CFLAGS"
			AC_DEFINE(WITH_PYTHON, 1, [define if You want python])
			have_python=yes
		    fi
		fi

		if test "x$have_python" != "xyes"; then 
			AC_MSG_RESULT(not found)
		fi
	fi
])

