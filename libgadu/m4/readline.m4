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

