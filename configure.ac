# configure.ac for Kadu written by A.Smarzewski
# Process this file with autoconf to produce a configure script.

AC_INIT([Kadu],[0.3.3-pre1])
AC_COPYRIGHT([configure.ac written by A.Smarzewski])
AC_REVISION([0.3.3-pre1])
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
AM_INIT_AUTOMAKE([kadu-0.3.3-pre1],[0.3.3-pre1])

KDE_SET_PREFIX


########################################################################
# check for programs, libraries and functions
########################################################################

CXXFLAGS=""

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# KDE and QT
KDE_PROG_LIBTOOL
AM_KDE_WITH_NLS
AC_PATH_KDE

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


########################################################################
# parse configuration options
########################################################################

MY_SUBDIRS="libgadu kadu po varia"

# doc support
AC_ARG_ENABLE(doc,[  --disable-doc           do not install Kadu documentation])
if test -z "$enable_doc"; then
	enable_doc=yes
fi
if test "$enable_doc" = "yes"; then
	AC_DEFINE(HAVE_DOC,1,[Define if Kadu documentation is installed])
	MY_SUBDIRS="$MY_SUBDIRS doc"
fi

# sms support
AC_ARG_ENABLE(sms,[  --disable-sms           do not compile and install built-in sms application])
if test -z "$enable_sms"; then
	enable_sms=yes
fi
if test "$enable_sms" = "yes"; then
	AC_DEFINE(HAVE_SMS,1,[Define if built-in sms application is compiled and installed])
	MY_SUBDIRS="$MY_SUBDIRS sms"
fi

# pthread support
AC_ARG_WITH(pthread,[  --without-pthread       do not use pthread in libgadu resolver])
if test -z "$with_pthread"; then
	with_pthread=yes
	ac_configure_args="$ac_configure_args --with-pthread"
fi

# debug support
if test -z "$enable_debug"; then
	enable_debug=no
fi
if test "$enable_debug" = "yes"; then
	CXXFLAGS=`echo "$CXXFLAGS" | sed s/-ansi//`
	CXXFLAGS=`echo "$CXXFLAGS" | sed s/-pedantic//`
	CXXFLAGS=`echo "$CXXFLAGS" | sed s/-W* //`
	CXXFLAGS="$CXXFLAGS -ggdb"
else
	ac_configure_args="$ac_configure_args --disable-debug"
fi

AC_SUBST(MY_SUBDIRS)


########################################################################
# configure libgadu and sms application
########################################################################

echo
echo " ***************************************************************"
echo " * Configuring libgadu and sms (if enabled), please wait . . . *"
echo " ***************************************************************"
echo

ac_configure_args="$ac_configure_args --disable-static --enable-shared"
AC_CONFIG_SUBDIRS([libgadu])
if test "$enable_sms" = "yes"; then
	AC_CONFIG_SUBDIRS([sms])
fi

########################################################################
# finalize configure
########################################################################

KDE_CREATE_SUBDIRSLIST

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
echo "    Installation prefix:                          $prefix" 
echo "    Install Kadu documentation:                   $enable_doc"
echo "    Compile and install built-in sms application: $enable_sms"
echo "    Use pthread resolving in libgadu:             $with_pthread"
echo "    Compile with debug symbols:                   $enable_debug"
echo
echo " Run make now (gmake on FreeBSD) to compile Kadu."
echo
