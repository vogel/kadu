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
dnl $Id: aclocal.m4,v 1.4 2002/11/15 00:24:41 adrian Exp $

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
    if test "x$ac_header_stdint" != "xstdint.h" ; then
      echo "#include <$ac_header_stdint>" > $ac_stdint_h
    fi
  else
    AC_MSG_RESULT([not found, using reasonable defaults])
    
    cat > $ac_stdint_h << EOF
#ifndef __AC_STDINT_H
#define __AC_STDINT_H 1

/* ISO C 9X: 7.18 Integer types <stdint.h> */

#define __int8_t_defined
typedef   signed char    int8_t;
typedef unsigned char   uint8_t;
typedef   signed short  int16_t;
typedef unsigned short uint16_t;
typedef   signed int    int32_t;
typedef unsigned int   uint32_t;

#endif /* __AC_STDINT_H */
EOF
  fi
])



dnl Rewritten from scratch. --wojtekka
dnl $Id: aclocal.m4,v 1.4 2002/11/15 00:24:41 adrian Exp $

AC_DEFUN(AC_CHECK_NCURSES,[
  AC_SUBST(CURSES_LIBS)
  AC_SUBST(CURSES_INCLUDES)

  AC_ARG_WITH(ncurses,
    [[  --with-ncurses[=dir]    Compile with ncurses/locate base dir]],
      if test "x$withval" = "xno" ; then
        without_ncurses=yes
      elif test "x$withval" != "xyes" ; then
        with_arg=$withval/include:-L$withval/lib
      fi)

  if test "x$without_ncurses" != "xyes" ; then
    AC_MSG_CHECKING(for ncurses.h)

    for i in $with_arg \
    		/usr/include: \
		/usr/local/include:"-L/usr/local/lib -L/usr/local/lib/ncurses" \
		/usr/pkg/include:-L/usr/pkg/lib \
		/usr/contrib/include:-L/usr/contrib/lib \
		/usr/freeware/include:-L/usr/freeware/lib32 \
    		/sw/include:-L/sw/lib \
    		/cw/include:-L/cw/lib; do
	
      incl=`echo "$i" | sed 's/:.*//'`
      lib=`echo "$i" | sed 's/.*://'`
		
      if test -f $incl/ncurses/ncurses.h; then
        include=$incl/ncurses
      elif test -f $incl/ncurses.h; then
        include=$incl
      fi

      if test "x$include" != "x"; then
        AC_MSG_RESULT($include/ncurses.h)
	CURSES_LIBS="$lib"
	CURSES_INCLUDES="-I$include"
	have_ncurses=true
	ldflags_old="$LDFLAGS"
	LDFLAGS="$CURSES_LIBS"
	AC_DEFINE(HAVE_NCURSES, 1, [define if You want ncurses])
	AC_CHECK_LIB(ncurses, initscr,
	  [CURSES_LIBS="$CURSES_LIBS -lncurses"],
	  [AC_CHECK_LIB(curses, initscr,
	    [CURSES_LIBS="$CURSES_LIBS -lcurses"])])
	LDFLAGS="$ldflags_old"
	break
      fi
    done
  fi

  if test "x$have_ncurses" != "xtrue"; then
    AC_MSG_RESULT(not found)
  fi
])



dnl Rewritten from scratch. --wojtekka
dnl $Id: aclocal.m4,v 1.4 2002/11/15 00:24:41 adrian Exp $

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
	     /net/caladium/usr/people/piotr.nba/temp/pkg/include:-L/net/caladium/usr/people/piotr.nba/temp/pkg/lib; do
    
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
        have_readline=true
        break
      elif test -f $incl/readline.h -a "x$incl" != "x/usr/include"; then
        AC_MSG_RESULT($incl/readline.h)
        READLINE_LIBS="$lib -lreadline"
        READLINE_INCLUDES="-I$incl"
        AC_DEFINE(HAVE_READLINE, 1, [define if You want readline])
        have_readline=true
        break
      fi
    done
  fi

  if test "x$have_readline" != "xtrue"; then
    AC_MSG_RESULT(not found)
  fi
])


dnl Rewritten from scratch. --speedy 
dnl $Id: aclocal.m4,v 1.4 2002/11/15 00:24:41 adrian Exp $

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
			PY_LIBS="$PY_LOCALMODLIBS $PY_BASEMODLIBS $PY_OTHER_LIBS $PY_OTHER_LIBC $PY_OTHER_LIBM"

			PYTHON_LIBS="-L$PY_EXEC_PREFIX/lib $PY_LIB_LOC -lpython$PY_VERSION $PY_LIBS"
			PYTHON_INCLUDES="$PY_CFLAGS"
			AC_DEFINE(WITH_PYTHON, 1, [define if You want python])
			have_python=true
		    fi
		fi

		if test "x$have_python" != "xtrue"; then 
			AC_MSG_RESULT(not found)
		fi
	fi
])

