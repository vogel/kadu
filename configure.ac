# Process this file with autoconf to produce a configure script.
AC_INIT([Kadu],[0.3.1])
AC_COPYRIGHT([configure.ac written by A.Smarzewski])
AC_REVISION([0.1.0])
AC_PREREQ(2.53)

AC_CONFIG_SRCDIR([kadu/kadu.cpp])
AC_CONFIG_AUX_DIR([admin])
AM_CONFIG_HEADER([config.h])
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE([kadu-3.0.1],[3.0.1])

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

KDE_CREATE_SUBDIRSLIST
AC_CONFIG_FILES([./Makefile kadu/Makefile po/Makefile varia/Makefile])
AC_CONFIG_SUBDIRS([libgadu])
AC_OUTPUT
