dnl Rewritten from scratch. --wojtekka
dnl $Id: curses.m4,v 1.8 2002/11/28 11:08:34 chilek Exp $

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


