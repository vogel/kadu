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
	AC_MSG_CHECKING([if lib$1.so/dylib library is in system directories])
	save_CFLAGS="$CFLAGS"
	CFLAGS="$CFLAGS -l$1"
	AC_TRY_LINK([],
	[
		return 0;
	],[
		CFLAGS="$save_CFLAGS"
		AC_MSG_RESULT([yes])
	],[
		CFLAGS="$save_CFLAGS"
		AC_MSG_RESULT([no])
		FIND_FILE([lib$1.so],[$2],[$3],
		[
			FIND_FILE([lib$1.dylib],[$2],[$3],[$4])
		])
	])
])

AC_DEFUN(FIND_AVAILABLE_MODULES,[
	available_modules=
	cd modules
	for dir in *; do \
		if test -f $dir/spec; then \
			available_modules="$available_modules $dir"; \
		fi; \
	done
	cd ..
])

AC_DEFUN(CHECK_MODULES_CONFIGURATION,[
	static_modules=
	shared_modules=
	disabled_modules=
	source .config; \
	for mod in $available_modules; do \
		echo "option=\$module_$mod" > .temp; \
		source .temp; \
		if test -z "$option"; then \
			option=m; \
		fi; \
		case "$option" in
			y) static_modules="$static_modules $mod";; \
			m) shared_modules="$shared_modules $mod";; \
			*) disabled_modules="$disabled_modules $mod";; \
		esac; \
	done; \
	rm .temp
])

AC_DEFUN(PROCESS_MODULE_SPEC_FILES,[
	echo
	echo " ******************************************"
	echo " * Configuring modules, please wait . . . *"
	echo " ******************************************"
	for mod in $shared_modules; do \
		if test -f modules/$mod/spec; then \
			echo; \
			echo ">>> Configuring $mod module..."; \
			MODULE_LIBS=; \
			MODULE_LIBS_PATH=; \
			MODULE_INCLUDES=; \
			MODULE_INCLUDES_PATH=; \
			source modules/$mod/spec; \
			echo "MODULE_LIBS:          $MODULE_LIBS"; \
			echo "MODULE_LIBS_PATH:     $MODULE_LIBS_PATH"; \
			echo "MODULE_INCLUDES:      $MODULE_INCLUDES"; \
			echo "MODULE_INCLUDES_PATH: $MODULE_INCLUDES_PATH"; \
			echo -n > modules/$mod/ldflags; \
			echo -n > modules/$mod/cxxflags; \
			for lib in $MODULE_LIBS; do \
				FIND_LIBRARY([$lib],[$MODULE_LIBS_PATH],
				[
					echo -n "-L$FILE_DIR " >> modules/$mod/ldflags
				]) \
				echo -n "-l$lib " >> modules/$mod/ldflags; \
			done; \
			for hdr in $MODULE_INCLUDES; do \
				FIND_HEADER([$hdr],[$MODULE_INCLUDES_PATH],
				[
					echo -n "-I$FILE_DIR " >> modules/$mod/cxxflags
				]) \
			done; \
		fi; \
	done
])

########################################################################

AC_INIT([Kadu],[])
AC_COPYRIGHT([configure.ac written by A.Smarzewski])
AC_REVISION([])
AC_PREREQ(2.53)

version=`cat VERSION`

echo
echo " ***************************************"
echo " * Configuring Kadu, please wait . . . *"
echo " ***************************************"

AC_CONFIG_SRCDIR([kadu/kadu.cpp])
AC_CONFIG_AUX_DIR([admin])
AM_CONFIG_HEADER([kadu-config.h])
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE([kadu-$version],[$version])

AC_DEFINE_UNQUOTED(BINDIR,"$prefix/bin",[Executable files prefix])
AC_DEFINE_UNQUOTED(DATADIR,"$prefix/share",[Data files prefix])

########################################################################
# check for programs, libraries and functions
########################################################################

user_cxxflags="$CXXFLAGS"
CXXFLAGS="-fno-exceptions -pipe $user_cxxflags"

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_AWK

##################
# PTHREAD Library
##################

CHECK_PTHREAD
AC_SUBST(PTHREAD_CPPFLAGS)
AC_SUBST(PTHREAD_LDFLAGS)
AC_SUBST(PTHREAD_LIBS)

#################
# PNG Library
#################

PNG_LIBS="-lpng"
FIND_LIBRARY([png],[/usr/lib /usr/local/lib],
[
	PNG_LIBS="$PNG_LIBS -L$FILE_DIR"
])
AC_SUBST(PNG_LIBS)

#################
# XFREE Headers
#################

FIND_HEADER([X11/Xlib.h],[/usr/X11R6/include /usr/include /usr/local/include],
[
	XFREE_INCLUDES="-I$FILE_DIR"
])
AC_SUBST(XFREE_INCLUDES)

#################
# QT Headers
#################

AC_ARG_WITH(qt-dir,        [  --with-qt-dir=DIR       qt-dir in DIR])
AC_ARG_WITH(qt-includes,   [  --with-qt-includes=DIR  qt-includes in DIR])

FIND_HEADER([qwidget.h],[$with_qt_includes $with_qt_dir/include $QTDIR/include /usr/lib/qt/include /usr/lib/qt3/include /usr/local/qt/include /usr/include /usr/include/qt /usr/include/qt3 /usr/local/include /usr/X11R6/include /usr/X11R6/include/qt /usr/X11R6/include/qt3 /sw/include/qt],
[
	QT_INCLUDES="-I$FILE_DIR"
])
AC_SUBST(QT_INCLUDES)

#################
# QT Library
#################

AC_ARG_WITH(qt-libs,       [  --with-qt-libs=DIR      qt-libs in DIR])

QT_LIBS="-lqt-mt"
FIND_LIBRARY([qt-mt],[$with_qt_libs $with_qt_dir/lib $QTDIR/lib /usr/lib/qt3/lib /usr/local/qt/lib /usr/lib/qt /usr/lib /usr/local/lib /usr/X11R6/lib /sw/lib],
[
	QT_LIBS="$QT_LIBS -L$FILE_DIR"
])
AC_SUBST(QT_LIBS)

#################
# QT MOC
#################

AC_ARG_WITH(qt-moc,        [  --with-qt-moc=DIR       qt-moc in DIR])

FIND_FILE([moc],[$with_qt_moc $with_qt_dir/bin $QTDIR/bin /usr/lib/qt3/bin /usr/local/qt/bin /usr/lib/qt/bin /usr/bin /bin /usr/local/bin /usr/X11R6/bin],
[
	QT_MOC_DIR="$FILE_DIR"
])
AC_SUBST(QT_MOC_DIR)

########################################################################

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h inttypes.h limits.h netinet/in.h stdint.h stdlib.h string.h sys/ioctl.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_FUNC_STAT
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([memset mkdir sqrt strdup])


########################################################################
# parse configuration options
########################################################################

MY_SUBDIRS=

# existing libgadu support
AC_ARG_WITH(existing-libgadu,[  --with-existing-libgadu do not compile libgadu, link with existing one])
if test -z "$with_existing_libgadu"; then
	with_existing_libgadu=no
fi
if test "$with_existing_libgadu" != "no"; then
	FIND_HEADER([libgadu.h],[$with_existing_libgadu $with_existing_libgadu/include],
	[
		GADU_INCLUDES="-I$FILE_DIR"
	])
	GADU_LIBS=-lgadu
	FIND_LIBRARY([gadu],[$with_existing_libgadu $with_existing_libgadu/lib],
	[
		GADU_LIBS="$GADU_LIBS -L$FILE_DIR"
	])
else
	MY_SUBDIRS="$MY_SUBDIRS libgadu"
	GADU_LIBS="../libgadu/lib/libgadu.a"
	GADU_INCLUDES="-I../libgadu/lib"
fi
AC_SUBST(GADU_LIBS)
AC_SUBST(GADU_INCLUDES)

# Voice talk support
AC_ARG_ENABLE(voice,[  --disable-voice         compile without voice talk support])
if test -z "$enable_voice"; then
	enable_voice=yes
fi
if test "$enable_voice" = "yes"; then
	AC_DEFINE(VOICE_ENABLED,1,[Define if voice talk support is enabled])
	GSM_LIBS="-L../libgsm/lib -lgsm"
	GSM_INCLUDES="-I../libgsm/inc"
	MY_SUBDIRS="$MY_SUBDIRS libgsm"
fi
AM_CONDITIONAL(VOICE,[test "$enable_voice" = "yes"])
AC_SUBST(GSM_LIBS)
AC_SUBST(GSM_INCLUDES)

MY_SUBDIRS="$MY_SUBDIRS kadu translations varia"

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
	AC_DEFINE(DEBUG_ENABLED,1,[Define if you want console debug output])
	CXXFLAGS="$CXXFLAGS -ggdb"
	libgadu_CFLAGS="-ggdb"
else
	CXXFLAGS="$CXXFLAGS -s"
	libgadu_CFLAGS="-DGG_DEBUG_DISABLE -s"
fi

# dist-info support
AC_ARG_ENABLE(dist-info,[  --enable-dist-info=DIST enable distribution type information, default: sources])
if test -z "$enable_dist_info"; then
	enable_dist_info=sources
fi
AC_DEFINE_UNQUOTED(DIST_TYPE,"$enable_dist_info",[Define distribution type of this copy of application])

# OpenSSL support
AC_CHECK_OPENSSL
if test -z "$have_openssl"; then
	have_openssl=no
fi
AM_CONDITIONAL(OPENSSL,[test "$have_openssl" = "yes"])
AC_SUBST(OPENSSL_LIBS)
AC_SUBST(OPENSSL_INCLUDES)

# Loadable modules support
AC_ARG_ENABLE(modules,[  --disable-modules       compile without loadable modules support])
if test -z "$enable_modules"; then
	enable_modules=yes
fi
if test "$enable_modules" = "yes"; then
	AC_DEFINE(MODULES_ENABLED,1,[Define if loadable modules support is enabled])
	MODULES_LDFLAGS=-rdynamic
	MY_SUBDIRS="$MY_SUBDIRS modules"
	FIND_AVAILABLE_MODULES()
	CHECK_MODULES_CONFIGURATION()
	PROCESS_MODULE_SPEC_FILES()
fi
AM_CONDITIONAL(MODULES,[test "$enable_modules" = "yes"])
AC_SUBST(MODULES_LDFLAGS)
AC_SUBST(shared_modules)
AC_SUBST(static_modules)

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
	ac_configure_args="$ac_configure_args 'CFLAGS_LIBGADU=$libgadu_CFLAGS' --enable-static --disable-shared"
	AC_CONFIG_SUBDIRS([libgadu])
fi

########################################################################
# finalize configure
########################################################################

AC_CONFIG_FILES([./Makefile kadu/Makefile translations/Makefile varia/Makefile modules/Makefile varia/scripts/kadu-config])
AC_OUTPUT

echo
echo " **************************************"
echo " * Kadu configuration is now complete *"
echo " **************************************"
echo
echo " Kadu was configured using options specified below:"
echo
echo "   Installation prefix:                [[$prefix]]" 
echo "   Use pthread resolving in libgadu:   $with_pthread"
echo "   Compile with debug symbols:         $enable_debug"
echo "   Link with existing libgadu:         $with_existing_libgadu"
echo "   Voice talk (libgsm) support:        $enable_voice"
echo "   Loadable modules support:           $enable_modules"
echo "   OpenSSL encryption support:         $have_openssl"
echo "   Distribution type information:      $enable_dist_info"
echo "   Additional c++ compiler flags:      $user_cxxflags"
echo

if test "$enable_modules" == "yes"; then \
	echo "   Static modules:   $static_modules"; \
	echo "   Shared modules:   $shared_modules"; \
	echo "   Disabled modules: $disabled_modules"; \
fi

echo
echo " Run make now (Linux) or gmake (FreeBSD and others) to compile Kadu."
echo
