# aclocal.m4 generated automatically by aclocal 1.6.2 -*- Autoconf -*-

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



dnl Curses detection: Munged from Midnight Commander's configure.in
dnl slightly modified for nicer output --wojtekka
dnl
dnl What it does:
dnl =============
dnl
dnl - Determine which version of curses is installed on your system
dnl   and set the -I/-L/-l compiler entries and add a few preprocessor
dnl   symbols 
dnl - Do an AC_SUBST on the CURSES_INCLUDEDIR and CURSES_LIBS so that
dnl   @CURSES_INCLUDEDIR@ and @CURSES_LIBS@ will be available in
dnl   Makefile.in's
dnl - Modify the following configure variables (these are the only
dnl   curses.m4 variables you can access from within configure.in)
dnl   CURSES_INCLUDEDIR - contains -I's and possibly -DRENAMED_CURSES if
dnl                       an ncurses.h that's been renamed to curses.h
dnl                       is found.
dnl   CURSES_LIBS       - sets -L and -l's appropriately
dnl   CFLAGS            - if --with-sco, add -D_SVID3 
dnl   has_curses        - exports result of tests to rest of configure
dnl
dnl Usage:
dnl ======
dnl 1) Add lines indicated below to acconfig.h
dnl 2) call AC_CHECK_CURSES after AC_PROG_CC in your configure.in
dnl 3) Instead of #include <curses.h> you should use the following to
dnl    properly locate ncurses or curses header file
dnl
dnl    #if defined(USE_NCURSES) && !defined(RENAMED_NCURSES)
dnl    #include <ncurses.h>
dnl    #else
dnl    #include <curses.h>
dnl    #endif
dnl
dnl 4) Make sure to add @CURSES_INCLUDEDIR@ to your preprocessor flags
dnl 5) Make sure to add @CURSES_LIBS@ to your linker flags or LIBS
dnl
dnl Notes with automake:
dnl - call AM_CONDITIONAL(HAS_CURSES, test "$has_curses" = true) from
dnl   configure.in
dnl - your Makefile.am can look something like this
dnl   -----------------------------------------------
dnl   INCLUDES= blah blah blah $(CURSES_INCLUDEDIR) 
dnl   if HAS_CURSES
dnl   CURSES_TARGETS=name_of_curses_prog
dnl   endif
dnl   bin_PROGRAMS = other_programs $(CURSES_TARGETS)
dnl   other_programs_SOURCES = blah blah blah
dnl   name_of_curses_prog_SOURCES = blah blah blah
dnl   other_programs_LDADD = blah
dnl   name_of_curses_prog_LDADD = blah $(CURSES_LIBS)
dnl   -----------------------------------------------
dnl
dnl
dnl The following lines should be added to acconfig.h:
dnl ==================================================
dnl
dnl /*=== Curses version detection defines ===*/
dnl /* Found some version of curses that we're going to use */
dnl #undef HAS_CURSES
dnl    
dnl /* Use SunOS SysV curses? */
dnl #undef USE_SUNOS_CURSES
dnl 
dnl /* Use old BSD curses - not used right now */
dnl #undef USE_BSD_CURSES
dnl 
dnl /* Use SystemV curses? */
dnl #undef USE_SYSV_CURSES
dnl 
dnl /* Use Ncurses? */
dnl #undef USE_NCURSES
dnl 
dnl /* If you Curses does not have color define this one */
dnl #undef NO_COLOR_CURSES
dnl 
dnl /* Define if you want to turn on SCO-specific code */
dnl #undef SCO_FLAVOR
dnl 
dnl /* Set to reflect version of ncurses *
dnl  *   0 = version 1.*
dnl  *   1 = version 1.9.9g
dnl  *   2 = version 4.0/4.1 */
dnl #undef NCURSES_970530
dnl
dnl /*=== End new stuff for acconfig.h ===*/
dnl 


AC_DEFUN(AC_CHECK_CURSES,[
	search_ncurses=true
	screen_manager=""
	has_curses=false

	CFLAGS=${CFLAGS--O}

	AC_SUBST(CURSES_LIBS)
	AC_SUBST(CURSES_INCLUDEDIR)

	AC_ARG_WITH(sco,
	  [  --with-sco              Use this to turn on SCO-specific code],[
	  if test x$withval = xyes; then
		AC_DEFINE(SCO_FLAVOR)
		CFLAGS="$CFLAGS -D_SVID3"
	  fi
	])

	AC_ARG_WITH(sunos-curses,
	  [  --with-sunos-curses     Used to force SunOS 4.x curses],[
	  if test x$withval = xyes; then
		AC_USE_SUNOS_CURSES
	  fi
	])

	AC_ARG_WITH(osf1-curses,
	  [  --with-osf1-curses      Used to force OSF/1 curses],[
	  if test x$withval = xyes; then
		AC_USE_OSF1_CURSES
	  fi
	])

	AC_ARG_WITH(vcurses,
	  [[  --with-vcurses[=incdir] Used to force SysV curses]],
	  if test x$withval != xyes; then
		CURSES_INCLUDEDIR="-I$withval"
	  fi
	  AC_USE_SYSV_CURSES
	)

	AC_ARG_WITH(ncurses,
	  [[  --with-ncurses[=dir]    Compile with ncurses/locate base dir]],
	  if test x$withval = xno ; then
		search_ncurses=false
	  elif test x$withval != xyes ; then
		AC_NCURSES($withval/include, ncurses.h, -L$withval/lib -lncurses, -I$withval/include, $withval/include)
	  fi
	)

	if $search_ncurses
	then
		AC_SEARCH_NCURSES()
	fi
])


AC_DEFUN(AC_USE_SUNOS_CURSES, [
	search_ncurses=false
	screen_manager="SunOS 4.x /usr/5include curses"
	AC_MSG_RESULT(Using SunOS 4.x /usr/5include curses)
	AC_DEFINE(USE_SUNOS_CURSES)
	AC_DEFINE(HAS_CURSES)
	has_curses=true
	AC_DEFINE(NO_COLOR_CURSES)
	AC_DEFINE(USE_SYSV_CURSES)
	CURSES_INCLUDEDIR="-I/usr/5include"
	CURSES_LIBS="/usr/5lib/libcurses.a /usr/5lib/libtermcap.a"
	AC_MSG_RESULT(Please note that some screen refreshs may fail)
])

AC_DEFUN(AC_USE_OSF1_CURSES, [
       AC_MSG_RESULT(Using OSF1 curses)
       search_ncurses=false
       screen_manager="OSF1 curses"
       AC_DEFINE(HAS_CURSES)
       has_curses=true
       AC_DEFINE(NO_COLOR_CURSES)
       AC_DEFINE(USE_SYSV_CURSES)
       CURSES_LIBS="-lcurses"
])

AC_DEFUN(AC_USE_SYSV_CURSES, [
	AC_MSG_RESULT(Using SysV curses)
	AC_DEFINE(HAS_CURSES)
	has_curses=true
	AC_DEFINE(USE_SYSV_CURSES)
	search_ncurses=false
	screen_manager="SysV/curses"
	CURSES_LIBS="-lcurses"
])

dnl AC_ARG_WITH(bsd-curses,
dnl [--with-bsd-curses         Used to compile with bsd curses, not very fancy],
dnl 	search_ncurses=false
dnl	screen_manager="Ultrix/cursesX"
dnl	if test $system = ULTRIX
dnl	then
dnl	    THIS_CURSES=cursesX
dnl        else
dnl	    THIS_CURSES=curses
dnl	fi
dnl
dnl	CURSES_LIBS="-l$THIS_CURSES -ltermcap"
dnl	AC_DEFINE(HAS_CURSES)
dnl	has_curses=true
dnl	AC_DEFINE(USE_BSD_CURSES)
dnl	AC_MSG_RESULT(Please note that some screen refreshs may fail)
dnl	AC_WARN(Use of the bsdcurses extension has some)
dnl	AC_WARN(display/input problems.)
dnl	AC_WARN(Reconsider using xcurses)
dnl)

	
dnl
dnl Parameters: directory filename cureses_LIBS curses_INCLUDEDIR nicename
dnl
AC_DEFUN(AC_NCURSES, [
    if $search_ncurses
    then
        if test -f $1/$2
	then
	    AC_MSG_RESULT($1/$2)

	    CURSES_LIBS="$3"
	    AC_CHECK_LIB(ncurses, initscr, , [
                CHECKLIBS=`echo "$3"|sed 's/-lncurses/-lcurses/g'`
		AC_CHECK_LIB(curses, initscr, [
			CURSES_LIBS="$CHECKLIBS"
		],, $CHECKLIBS)
	    ], $CURSES_LIBS)
	    CURSES_INCLUDEDIR="$4"
	    search_ncurses=false
	    screen_manager=$5
            AC_DEFINE(HAS_CURSES)
            has_curses=true
	    has_ncurses=true
	    AC_DEFINE(USE_NCURSES)
	fi
    fi
])

AC_DEFUN(AC_SEARCH_NCURSES, [
    AC_MSG_CHECKING(for location of ncurses.h)

    AC_NCURSES(/usr/include, ncurses.h, -lncurses,, /usr/include)
    AC_NCURSES(/usr/include/ncurses, ncurses.h, -lncurses, -I/usr/include/ncurses, /usr/include/ncurses)
    AC_NCURSES(/usr/local/include, ncurses.h, -L/usr/local/lib -lncurses, -I/usr/local/include, /usr/local)
    AC_NCURSES(/usr/pkg/include, ncurses.h, -L/usr/pkg/lib -lncurses, -I/usr/pkg/include, /usr/pkg)
    AC_NCURSES(/usr/contrib/include, ncurses.h, -L/usr/contrib/lib -lncurses, -I/usr/contrib/include, /usr/contrib)
    AC_NCURSES(/usr/local/include/ncurses, ncurses.h, -L/usr/local/lib -L/usr/local/lib/ncurses -lncurses, -I/usr/local/include/ncurses, /usr/local/include/ncurses)
    AC_NCURSES(/usr/freeware/include/ncurses, curses.h, -L/usr/freeware/lib32 -lncurses, -I/usr/freeware/include, /usr/freeware)
    
    AC_NCURSES(/usr/local/include/ncurses, curses.h, -L/usr/local/lib -lncurses, -I/usr/local/include/ncurses -DRENAMED_NCURSES, renamed ncurses on /usr/local/.../ncurses)

    AC_NCURSES(/usr/include/ncurses, curses.h, -lncurses, -I/usr/include/ncurses -DRENAMED_NCURSES, renamed ncurses on /usr/include/ncurses)
    AC_NCURSES(/sw/include, ncurses.h, -L/sw/lib -lncurses, -I/sw/include, macosx)
    
    dnl
    dnl We couldn't find ncurses, try SysV curses
    dnl
    if $search_ncurses 
    then
        AC_EGREP_HEADER(init_color, /usr/include/curses.h,
	    AC_USE_SYSV_CURSES)
	AC_EGREP_CPP(USE_NCURSES,[
#include <curses.h>
#ifdef __NCURSES_H
#undef USE_NCURSES
USE_NCURSES
#endif
],[
	CURSES_INCLUDEDIR="$CURSES_INCLUDEDIR -DRENAMED_NCURSES"
        AC_DEFINE(HAS_CURSES)
	has_curses=true
	has_ncurses=true
        AC_DEFINE(USE_NCURSES)
        search_ncurses=false
        screen_manager="ncurses installed as curses"
])
    fi

    dnl
    dnl Try SunOS 4.x /usr/5{lib,include} ncurses
    dnl The flags USE_SUNOS_CURSES, USE_BSD_CURSES and BUGGY_CURSES
    dnl should be replaced by a more fine grained selection routine
    dnl
    if $search_ncurses
    then
	if test -f /usr/5include/curses.h
	then
	    AC_USE_SUNOS_CURSES
        fi
    fi

    dnl use whatever curses there happens to be
    if $search_ncurses
    then
	if test -f /usr/include/curses.h
	then
	  CURSES_LIBS="-lcurses"
	  AC_DEFINE(HAS_CURSES)
	  has_curses=true
	  search_ncurses=false
	  screen_manager="curses"
	fi
    fi
])


# Define a conditional.

AC_DEFUN(AM_CONDITIONAL,
[AC_SUBST($1_TRUE)
AC_SUBST($1_FALSE)
if $2; then
  $1_TRUE=
  $1_FALSE='#'
else
  $1_TRUE='#'
  $1_FALSE=
fi])

dnl readline detection
dnl based on curses.m4 from gnome
dnl slightly modified for nicer output --wojtekka
dnl
dnl What it does:
dnl =============
dnl
dnl - Determine which version of readline is installed on your system
dnl   and set the -I/-L/-l compiler entries and add a few preprocessor
dnl   symbols 
dnl - Do an AC_SUBST on the READLINE_INCLUDES and READLINE_LIBS so that
dnl   @READLINE_INCLUDES@ and @READLINE_LIBS@ will be available in
dnl   Makefile.in's
dnl - Modify the following configure variables (these are the only
dnl   readline.m4 variables you can access from within configure.in)
dnl   READLINE_INCLUDES - contains -I's
dnl   READLINE_LIBS       - sets -L and -l's appropriately
dnl   has_readline        - exports result of tests to rest of configure
dnl
dnl Usage:
dnl ======
dnl 1) Add lines indicated below to acconfig.h
dnl 2) call AC_CHECK_READLINE after AC_PROG_CC in your configure.in
dnl 3) Make sure to add @READLINE_INCLUDES@ to your preprocessor flags
dnl 4) Make sure to add @READLINE_LIBS@ to your linker flags or LIBS
dnl
dnl Notes with automake:
dnl - call AM_CONDITIONAL(HAVE_READLINE, test "$has_readline" = true) from
dnl   configure.in
dnl - your Makefile.am can look something like this
dnl   -----------------------------------------------
dnl   INCLUDES= blah blah blah $(READLINE_INCLUDES) 
dnl   if HAVE_READLINE
dnl   READLINE_TARGETS=name_of_readline_prog
dnl   endif
dnl   bin_PROGRAMS = other_programs $(READLINE_TARGETS)
dnl   other_programs_SOURCES = blah blah blah
dnl   name_of_readline_prog_SOURCES = blah blah blah
dnl   other_programs_LDADD = blah
dnl   name_of_readline_prog_LDADD = blah $(READLINE_LIBS)
dnl   -----------------------------------------------
dnl
dnl
dnl The following lines should be added to acconfig.h:
dnl ==================================================
dnl
dnl #undef HAVE_READLINE
dnl 
dnl /*=== End new stuff for acconfig.h ===*/
dnl 


AC_DEFUN(AC_CHECK_READLINE,[
	search_readline=true
	has_readline=false

dnl	CFLAGS=${CFLAGS--O}

	AC_SUBST(READLINE_LIBS)
	AC_SUBST(READLINE_INCLUDES)

	AC_ARG_WITH(readline,
	  [[  --with-readline[=dir]   Compile with readline/locate base dir]],
	  if test "x$withval" = "xno" ; then
		search_readline=false
	  elif test "x$withval" != "xyes" ; then
		READLINE_LIBS="$LIBS -L$withval/lib -lreadline"
		READLINE_INCLUDES="-I$withval/include -I$withval/include/readline -I$withval/readline"
		search_readline=false
		AC_DEFINE(HAVE_READLINE)
		has_readline=true
	  else
	  	search_readline=true
	  fi
	)

	if $search_readline
	then
		AC_SEARCH_READLINE()
	fi


])
	
dnl
dnl Parameters: directory filename cureses_LIBS curses_INCLUDES nicename
dnl
AC_DEFUN(AC_READLINE, [
    if $search_readline
    then
        if test -f $1/$2
	then
	    AC_MSG_RESULT($1/$2)
 	    READLINE_LIBS="$3"
	    READLINE_INCLUDES="$4"
	    search_readline=false
            AC_DEFINE(HAVE_READLINE)
            has_readline=true
	fi
    fi
])

AC_DEFUN(AC_SEARCH_READLINE, [
    AC_MSG_CHECKING(for location of readline.h)

    AC_READLINE(/usr/include, readline.h, -lreadline,, /usr/include)
    AC_READLINE(/usr/include/readline, readline.h, -lreadline, -I/usr/include/readline, /usr/include/readline)
    AC_READLINE(/usr/local/include, readline.h, -L/usr/local/lib -lreadline, -I/usr/local/include, /usr/local)
    AC_READLINE(/usr/local/include/readline, readline.h, -L/usr/local/lib -L/usr/local/lib/readline -lreadline, -I/usr/local/include/readline, /usr/local/include/readline)
    AC_READLINE(/usr/freeware/include/readline, readline.h, -L/usr/freeware/lib32 -lreadline, -I/usr/freeware/include, /usr/freeware/include/readline)
    AC_READLINE(/sw/include/readline, readline.h, -L/sw/lib -lreadline, -I/sw/include/readline -I/sw/include, /sw/include/readline)
] ) 


