# configure.ac for Kadu written by A.Smarzewski
# Process this file with autoconf to produce a configure script.

########################################################################
# autoconf macros
########################################################################

AC_DEFUN(CHECK_PTHREAD,
[
	AC_CHECK_LIB(pthread,pthread_create,
	[
		PTHREAD_CPPFLAGS=
		PTHREAD_LDFLAGS=
		PTHREAD_LIBS=-lpthread
	],[
		AC_MSG_CHECKING(if compiler supports -pthread)
		save_CPPFLAGS="$CPPFLAGS"
		CPPFLAGS="$CPPFLAGS -pthread"
		AC_TRY_LINK(
		[
			#include <pthread.h>
		],[
			pthread_create(0,0,0,0);
		],
		[
			AC_MSG_RESULT(yes)
			PTHREAD_CPPFLAGS=-pthread
			PTHREAD_LDFLAGS=-pthread
			PTHREAD_LIBS=
		],[
			AC_MSG_RESULT(no)
			AC_MSG_ERROR([Your system is not supporting pthreads!])
		])
		CPPFLAGS="$save_CPPFLAGS"
	])
])

# $1 - file name
# $2 - directories
# $3 - action if found
# $4 - action if not found (default is error message)
AC_DEFUN(FIND_FILE,[
	AC_MSG_CHECKING([for $1 in specified directiories])
	FILE_DIR=
	for dir in $2
	do
		if test -f $dir/$1
		then
			FILE_DIR=$dir
			AC_MSG_RESULT([found in $FILE_DIR])
			$3
			break
		fi
	done
	if test "X$FILE_DIR" = "X"
	then
		AC_MSG_RESULT([not found])
		if test "X$4" = "X"
		then
			AC_MSG_ERROR([$1 not found!])
		fi
		$4
	fi
])

# $1 - header name
# $2 - directories
# $3 - action if found but not in system dirs
# $4 - action if not found (default is error message)
AC_DEFUN(FIND_HEADER,[
	AC_MSG_CHECKING([if $1 header is in system directories])
	AC_TRY_COMPILE(
	[
		#include <$1>
	],[
		return 0;
	],[
		AC_MSG_RESULT([yes])
	],[
		AC_MSG_RESULT([no])
		FIND_FILE([$1],[$2],[$3],[$4])
	])
])

# $1 - library name (without "lib" prefix and extension)
# $2 - directories
# $3 - action if found but not in system dirs
# $4 - action if not found (default is error message)
AC_DEFUN(FIND_LIBRARY,[
	AC_MSG_CHECKING([if lib$1.so library is in system directories])
	save_CPPFLAGS="$CPPFLAGS"
	CPPFLAGS="$CXXFLAGS -l$1"
	AC_TRY_LINK([],
	[
		return 0;
	],[
		CPPFLAGS="$save_CPPFLAGS"
		AC_MSG_RESULT([yes])
	],[
		CPPFLAGS="$save_CPPFLAGS"
		AC_MSG_RESULT([no])
		FIND_FILE([lib$1.so],[$2],[$3],[$4])
	])
])

AC_DEFUN(FIND_MOC,[
	AC_MSG_CHECKING([for QT moc])

	for dir in $1
	do
		if test -f $dir/moc
		then
			QTMOCDIR=$dir
			AC_MSG_RESULT([found in $QTMOCDIR])
			break
		else
			if test -f $dir/bin/moc
			then
				QTMOCDIR=$dir/bin
				AC_MSG_RESULT([found in $QTMOCDIR])
				break
			fi
		fi
	done
])

AC_DEFUN(FIND_QT_LIBS,[
	AC_MSG_CHECKING([for QT libraries])

	if test "$enable_sharp" = "yes"
	then
		QTLIBS="qpe"
	else
		QTLIBS="qt2-mt qt-mt"
	fi

	for dir in $1
	do
		for i in $QTLIBS
		do
			if test -f $dir/lib$i.so
			then
				QTLIBDIR=$dir
				AC_MSG_RESULT([found in $QTLIBDIR])
				QTMT=$i
				break 2
			else
				if test -f $dir/lib/lib$i.so
				then
					QTLIBDIR=$dir/lib
					AC_MSG_RESULT([found in $QTLIBDIR])
					QTMT=$i
					break 2
				fi
			fi
		done
	done
])

AC_DEFUN(FIND_QT_INCLUDES,[
	AC_MSG_CHECKING([for QT include files])

	for dir in $1
	do
		if test -f $dir/qwidget.h
		then
			QTINCDIR=$dir
			AC_MSG_RESULT([found in $QTINCDIR])
			break
		else
			if test -f $dir/include/qwidget.h
			then
				QTINCDIR=$dir/include
				AC_MSG_RESULT([found in $QTINCDIR])
				break
			fi
		fi
	done
])

########################################################################

AC_INIT([Kadu],[0.3.3-rc2])
AC_COPYRIGHT([configure.ac written by A.Smarzewski])
AC_REVISION([0.3.3-rc2])
AC_PREREQ(2.53)

echo
echo " ***************************************"
echo " * Configuring Kadu, please wait . . . *"
echo " ***************************************"
echo

AC_CONFIG_SRCDIR([kadu/kadu.cpp])
AC_CONFIG_AUX_DIR([admin])
AM_CONFIG_HEADER([config.h])
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE([kadu-0.3.3-rc2],[0.3.3-rc2])


########################################################################
# check for programs, libraries and functions
########################################################################

CXXFLAGS=""

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
# AM_PROG_LIBTOOL

# pthread

CHECK_PTHREAD
AC_SUBST(PTHREAD_CPPFLAGS)
AC_SUBST(PTHREAD_LDFLAGS)
AC_SUBST(PTHREAD_LIBS)

# png

PNG_LIBS="-lpng"
FIND_LIBRARY([png],[/usr/lib /usr/local/lib],
[
	PNG_LIBS="$PNG_LIBS -L$FILE_DIR"
])
AC_SUBST(PNG_LIBS)

# QT

AC_ARG_WITH(qt-dir,        [  --with-qt-dir=DIR       qt-dir in DIR])
AC_ARG_WITH(qt-moc,        [  --with-qt-moc=DIR       qt-moc in DIR])
AC_ARG_WITH(qt-includes,   [  --with-qt-includes=DIR  qt-includes in DIR])
AC_ARG_WITH(qt-libs,       [  --with-qt-libs=DIR      qt-libs in DIR])

FIND_MOC($with_qt_moc $with_qt_dir $QTDIR /usr/lib/qt3 /usr/lib/qt2 /usr/local/qt /usr/lib/qt /usr/bin /bin /usr/local /usr/X11R6)
if test "X$QTMOCDIR" = "X"
then
	echo ""
	echo "Qt's moc not found! If you have installed Qt in an"
	echo "unusual place, please use the \"--with-qt-moc=\"" option
	echo ""
	exit
fi

FIND_QT_LIBS($with_qt_libs $with_qt_dir $QTDIR /usr/lib/qt3 /usr/lib/qt2 /usr/local/qt /usr/lib/qt /usr/lib /usr/local/lib /usr/X11R6/lib)
if test "X$QTLIBDIR" = "X"
then
	echo ""
	echo "Qt libs not found! If you have installed Qt in an"
	echo "unusual place, please use the \"--with-qt-libs=\"" option
	echo ""
	exit
else
	QTLIBDIR="-L$QTLIBDIR"
fi

FIND_QT_INCLUDES($with_qt_includes $with_qt_dir $QTDIR /usr/lib/qt3 /usr/lib/qt2 /usr/local/qt /usr/lib/qt /usr/include /usr/include/qt /usr/local/include /usr/X11R6/include /usr/X11R6/include/qt2 /usr/X11R6/include/qt3)
if test "X$QTINCDIR" = "X"
then
	echo ""
	echo "Qt headers not found! If you have installed Qt in an"
	echo "unusual place, please use the \"--with-qt-includes=\"" option
	echo ""
	exit
else
	QTINCDIR="-I$QTINCDIR"
fi

AC_SUBST(QTMOCDIR)
AC_SUBST(QTMT)
AC_SUBST(QTLIBDIR)
AC_SUBST(QTINCDIR)


# Checks for header files.
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netinet/in.h stdlib.h string.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_PID_T
AC_HEADER_TIME
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_STAT
AC_FUNC_STRFTIME
AC_FUNC_WAIT3
AC_CHECK_FUNCS([floor inet_ntoa memset mkdir select strchr strdup strerror strtol])

# gettext

FIND_HEADER([libintl.h],[/usr/include /usr/local/include],
[
	CPPFLAGS="$CPPFLAGS -I$FILE_DIR"
])
FIND_LIBRARY([intl],[/usr/lib /usr/local/lib],
[
	LIBS="$LIBS -L$FILE_DIR"
],[ ])
AM_GNU_GETTEXT([external])

########################################################################
# parse configuration options
########################################################################

MY_SUBDIRS=

# existing libgadu support
AC_ARG_WITH(existing-libgadu,[  --with-existing-libgadu do not compile libgadu, link with existing one])
if test -z "$with_existing_libgadu"; then
	with_existing_libgadu=no
fi
if test "$with_existing_libgadu" = "yes"; then
	AC_CHECK_HEADER(libgadu.h,[
		AC_CHECK_LIB(gadu,gg_libgadu_version,,[
			AC_MSG_ERROR([existing libgadu was not found!])
		],[-lpthread])
	],[
		AC_MSG_ERROR([existing libgadu was not found!])
	])
else
	MY_SUBDIRS="$MY_SUBDIRS libgadu"
	LIBS="$LIBS -L../libgadu/lib"
	CPPFLAGS="$CPPFLAGS -I../libgadu/lib"
fi

MY_SUBDIRS="$MY_SUBDIRS kadu po varia"

# doc support
AC_ARG_ENABLE(doc,[  --enable-doc=DIR        install doc in DIR [[DATADIR/apps/kadu/doc]]])
AC_ARG_ENABLE(doc,[  --disable-doc           do not install documentation])
if test -z "$enable_doc"; then
	enable_doc=yes
fi
if test "$enable_doc" != "no"; then
	AC_DEFINE(HAVE_DOC,1,[Define if documentation is installed])
	MY_SUBDIRS="$MY_SUBDIRS doc"
	if test "$enable_doc" = "yes"; then
		docdir="$datadir/apps/kadu/doc"
	else
		docdir="$enable_doc"
		enable_doc=yes
	fi;
fi
AC_SUBST(docdir)

# pthread support
AC_ARG_WITH(pthread,[  --without-pthread       do not use pthread in libgadu resolver])
if test -z "$with_pthread"; then
	with_pthread=yes
	ac_configure_args="$ac_configure_args --with-pthread"
fi

# debug support
AC_ARG_ENABLE(debug,[  --enable-debug          compile with debug symbols])
if test -z "$enable_debug"; then
	enable_debug=no
fi
if test "$enable_debug" = "yes"; then
	CXXFLAGS="$CXXFLAGS -ggdb"
else
	ac_configure_args="$ac_configure_args --disable-debug"
fi

# dist-info support
AC_ARG_ENABLE(dist-info,[  --enable-dist-info=DIST enable distribution type information, default: sources])
if test -z "$enable_dist_info"; then
	enable_dist_info=sources
fi
AC_DEFINE_UNQUOTED(DIST_TYPE,"$enable_dist_info",[Define distribution type of this copy of application])

# OpenSSL support
AC_ARG_WITH(openssl,[  --without-openssl       disable OpenSSL encryption support])
if test -z "$with_openssl"; then
	with_openssl=yes
fi
if test "$with_openssl" = "yes"; then
	AC_CHECK_HEADER(openssl/rsa.h,[
		AC_CHECK_LIB(crypto,RSA_public_encrypt,,[
			AC_MSG_ERROR([libcrypto was not found!])
		])
		AC_CHECK_LIB(ssl,BIO_new,,[
			AC_MSG_ERROR([libssl was not found!])
		])
	],[
		AC_MSG_ERROR([openssl/rsa.h was not found!])
	])
	AC_DEFINE(HAVE_OPENSSL,1,[Define if we have OpenSSL encryption support])
fi
AM_CONDITIONAL(OPENSSL,[test "$with_openssl" = "yes"])

AC_SUBST(MY_SUBDIRS)


########################################################################
# configure libgadu
########################################################################

echo
echo " ******************************************"
echo " * Configuring libgadu, please wait . . . *"
echo " ******************************************"
echo

if test "$with_existing_libgadu" = "no"; then
	ac_configure_args="$ac_configure_args --disable-static --enable-shared"
	AC_CONFIG_SUBDIRS([libgadu])
fi

########################################################################
# finalize configure
########################################################################

AC_CONFIG_FILES([./Makefile kadu/Makefile po/Makefile varia/Makefile])
if test "$enable_doc" = "yes"; then
	AC_CONFIG_FILES([doc/Makefile])
fi
AC_OUTPUT

echo
echo " **************************************"
echo " * Kadu configuration is now complete *"
echo " **************************************"
echo
echo " Kadu was configured using options specified below:"
echo
echo "   Installation prefix:                [[$prefix]]" 
echo "   Install Kadu documentation:         $enable_doc [[$docdir]]"
echo "   Use pthread resolving in libgadu:   $with_pthread"
echo "   Compile with debug symbols:         $enable_debug"
echo "   Link with existing libgadu:         $with_existing_libgadu"
echo "   OpenSSL encryption support:         $with_openssl"
echo "   Distribution type information:      $enable_dist_info"
echo
echo " Run make now (Linux) or gmake (FreeBSD and others) to compile Kadu."
echo
