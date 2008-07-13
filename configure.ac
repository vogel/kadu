
# $1 - modules to be browsed
# $2 - string of selected modules (shared or static)
# $3 - "shared" or "static" (depending on what is being checked)
AC_DEFUN(BROWSE_MODULES,[
	for mod in $1; do \
		second_field=\$\2; \
		echo -n "Dependencies=\"" > .temp; \
		$AWK -F '=' "/^Dependencies/ { printf $second_field }" modules/$mod/$mod.desc >> .temp; \
		echo "\"" >> .temp; \
		. ./.temp; \
		if test -n "$Dependencies"; then \
			 for checked_module in $Dependencies; do \
				 is_ok="false"; \
				 for one_of_modules in $2; do \
					 if test "$one_of_modules" = "$checked_module"; then \
						 is_ok="true"; \
					 fi; \
				 done; \
				 if test "$is_ok" = "false"; then \
					 AC_MSG_RESULT([failed]); \
					 str="Module $mod selected as $3. It depends on $checked_module, however"; \
					 str="$str $checked_module was not selected"; \
					 if test "$3" = "static"; then \
						 str="$str as static!"; \
					 else \
						 str="$str at all!"; \
					 fi; \
					 AC_MSG_ERROR([$str]); \
				 fi; \
			 done; \
		 fi; \
	 done; \
	 rm .temp; \
])

AC_DEFUN(CHECK_MODULES_DEPENDENCIES,[
	AC_MSG_CHECKING([modules dependencies]); \
	if test -n "$static_modules"; then \
		BROWSE_MODULES([$static_modules],[$static_modules],[static])
	fi; \

	if test -n "$shared_modules"; then \
		BROWSE_MODULES([$shared_modules],[$static_modules $shared_modules],[shared])
	fi; \
	AC_MSG_RESULT([ok])
])

AC_DEFUN(PROCESS_ICON_THEMES,[
	echo
	echo " **********************************************"
	echo " * Configuring icon themes, please wait . . . *"
	echo " **********************************************"
	if test $enable_autodownload = "yes" ; then \
		for i_t in $enabled_icon_themes; do \
			if test -f varia/themes/icons/$i_t.web; then \
				if test -f varia/themes/icons/$i_t/.autodownloaded -o ! -f varia/themes/icons/$i_t/icons.conf; then \
					cd varia/themes/icons; \
					if ! ../../scripts/autodownload $i_t "icon theme"; then \
						AC_MSG_ERROR([$i_t download failed!]); \
					fi; \
					cd ../../..; \
				fi; \
			fi; \
		done; \
	fi
])

AC_DEFUN(PROCESS_EMOTICON_THEMES,[
	echo
	echo " **************************************************"
	echo " * Configuring emoticon themes, please wait . . . *"
	echo " **************************************************"
	if test $enable_autodownload = "yes" ; then \
		for e_t in $enabled_emoticon_themes; do \
			if test -f varia/themes/emoticons/$e_t.web; then \
				if test -f varia/themes/emoticons/$e_t/.autodownloaded -o ! -f varia/themes/emoticons/$e_t/1/emots.txt; then \
					cd varia/themes/emoticons; \
					if ! ../../scripts/autodownload $e_t "emoticon theme"; then \
						AC_MSG_ERROR([$e_t download failed!]); \
					fi; \
					cd ../../..; \
				fi; \
			fi; \
		done; \
	fi
])

AC_DEFUN(GENERATE_STATIC_MODULES_CODE,[
	echo
	echo ">>> Generating kernel code for static modules ..."
	echo
	STATIC_MODULES_DECLS=
	STATIC_MODULES_REGRS=
	for mod in $static_modules; do \
		STATIC_MODULES_DECLS="$STATIC_MODULES_DECLS extern \"C\" int ${mod}_init(void); extern \"C\" void ${mod}_close(void);"; \
		STATIC_MODULES_REGRS="$STATIC_MODULES_REGRS registerStaticModule(\"${mod}\",${mod}_init,${mod}_close);"; \
	done
])

########################################################################

m4_include([version.m4])
AC_INIT([Kadu],VERSION_NUMBER)
AC_COPYRIGHT([configure.ac written by A.Smarzewski])
AC_REVISION([])
AC_PREREQ(2.53)
AC_LANG(C++)

echo
echo " ***************************************"
echo " * Configuring Kadu, please wait . . . *"
echo " ***************************************"

AC_CONFIG_SRCDIR([kadu-core/kadu.cpp])
AC_CONFIG_AUX_DIR([admin])
AM_CONFIG_HEADER([kadu-config.tmp:kadu-config.h.in])
AM_MAINTAINER_MODE
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE([no-dependencies dist-bzip2])

########################################################################
# check for programs, libraries and functions
########################################################################

user_cxxflags="$CXXFLAGS"
CXXFLAGS="-fno-exceptions -pipe -Wall $user_cxxflags -DQT3_SUPPORT=defined -DQT_COMPAT=defined"

user_cflags="$CFLAGS"
CFLAGS="-pipe -Wall $user_cflags"

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_AWK

###################
# Additional paths
###################

AC_ARG_WITH(includes,   [  --with-includes="DIRS"  search for includes in DIRS])
AC_ARG_WITH(libs,       [  --with-libs="DIRS"      search for libs in DIRS])
AC_ARG_WITH(apps,       [  --with-apps="DIRS"      search for apps in DIRS])

##################
# ExecInfo
##################

AC_CHECK_HEADERS([execinfo.h],
[
	AC_MSG_CHECKING([if execinfo is in libc])
	TRY_LINK_EXECINFO(
	[
		AC_MSG_RESULT([yes])
		AC_DEFINE(HAVE_EXECINFO,1,[Define if you want execinfo support])
	],[
		AC_MSG_RESULT([no])
		AC_MSG_CHECKING([if execinfo is in libexecinfo])
		save_LDLAGS="$LDLAGS"
		LDFLAGS="$LDFLAGS -lexecinfo"
		TRY_LINK_EXECINFO(
		[
			AC_MSG_RESULT([yes])
			AC_DEFINE(HAVE_EXECINFO,1,[Define if you want execinfo support])
		],[
			AC_MSG_RESULT([no])
			LDFLAGS="$save_LDLAGS"
		])
	])
])

##################
# PTHREAD Library
##################

CHECK_PTHREAD
AC_SUBST(PTHREAD_CPPFLAGS)
AC_SUBST(PTHREAD_LDFLAGS)
AC_SUBST(PTHREAD_LIBS)


########
# PATHS
########
QT_LIBS_PATHS="    $QTDIR/lib     /usr/lib/qt4/lib     /usr/lib/qt/lib     /usr/local/qt/lib     /usr/local/lib     /usr/lib      /usr/lib/qt     /usr/lib/qt4     /usr/share/qt4/lib     /usr/X11R6/lib     /sw/lib        /usr/lib/qt4     /usr/X11R6/lib/qt     /usr/X11R6/lib/qt4     /usr/lib/qt4/lib64  $QTDIR/lib64"
QT_INCLUDES_PATHS="$QTDIR/include /usr/lib/qt4/include /usr/lib/qt/include /usr/local/qt/include /usr/local/include /usr/include  /usr/include/qt /usr/include/qt4 /usr/share/qt4/include /usr/X11R6/include /sw/include/qt /usr/include/qt4 /usr/X11R6/include/qt /usr/X11R6/include/qt4"
QT_TOOLS_PATHS="   $QTDIR/bin     /usr/lib/qt4/bin     /usr/lib/qt/bin     /usr/local/qt/bin     /usr/local/bin     /usr/bin      /bin            /usr/share/qt4/bin     /usr/X11R6/bin"
PNG_LIBS_PATHS="   /usr/lib64     /usr/lib             /usr/local/lib      /sw/lib"
Z_LIBS_PATHS="     /usr/lib64     /usr/lib             /usr/local/lib      /sw/lib"


#################
# PNG Library
#################

AC_ARG_WITH(png-checking,[  --without-png-checking       do not check for libpng])
if test -z "$with_png_checking"; then
	with_png_checking=yes
fi
if test "$with_png_checking" = "yes"; then

	PNG_LIBS="-lpng"
	FIND_LIBRARY([png],[$PNG_LIBS_PATHS],
	[
		PNG_LIBS="$PNG_LIBS -L$FILE_DIR"
	],[
		AC_MSG_RESULT([libpng not found - checking for zlib first])

		#################
		# LibZ
		#################
		Z_LIBS="-lz"
		FIND_LIBRARY([z],[$Z_LIBS_PATHS],
		[
			Z_LIBS="$Z_LIBS -L$FILE_DIR"
		],[
			AC_MSG_ERROR([libz not found - please install zlib-devel])
		])
		AC_SUBST(Z_LIBS)
		AC_MSG_RESULT([zlib found - checking for libpng once again])

		FIND_LIBRARY([png],[$PNG_LIBS_PATHS],
		[
			PNG_LIBS="$Z_LIBS $PNG_LIBS -L$FILE_DIR"
		],[
			AC_MSG_ERROR([libpng not found - please install libpng-devel])
		],[$Z_LIBS -lm])
	])
	AC_SUBST(PNG_LIBS)

fi

#################
# QT Headers
#################

FIND_HEADER([QtGui/QWidget],[$QT_INCLUDES_PATHS],
[
	QT_INCLUDES="-I$FILE_DIR -I$FILE_DIR/Qt -I$FILE_DIR/QtCore -I$FILE_DIR/QtGui -I$FILE_DIR/Qt3Support -I$FILE_DIR/QtNetwork -I$FILE_DIR/QtXml"
],[
	AC_MSG_ERROR([libqt headers not found - please install libqt4-devel (>=4.3 <5.0) - check: http://kadu.net/qt])
])
AC_SUBST(QT_INCLUDES)

#################
# QT Library
#################


QT_LIBS="-lQtCore -lQtGui -lQt3Support -lQtNetwork"
FIND_LIBRARY([QtCore],[$QT_LIBS_PATHS],
[
	QT_LIBS="$QT_LIBS -L$FILE_DIR"
],[
	AC_MSG_ERROR([libqt-mt not found - please install libqt4 (>=4.3 <5.0) - check: http://kadu.net/qt])
],[$PNG_LIBS $PTHREAD_LDFLAGS $PTHREAD_LIBS])
AC_SUBST(QT_LIBS)


AC_MSG_CHECKING(for Qt version)
save_CXXFLAGS="$CXXFLAGS"
CXXFLAGS="$QT_INCLUDES"
AC_TRY_LINK(
[
	#include <qglobal.h>
],[
	#if QT_VERSION < 0x040300
		#error
	#endif
	#if QT_VERSION >= 0x050000
		#error
	#endif
],
[
	AC_MSG_RESULT(ok)

],[
	AC_MSG_RESULT(bad)
	AC_MSG_ERROR([Your Qt version is not appropiate! Get Qt >= 4.3 and < 5.0])
])
CXXFLAGS="$save_CXXFLAGS"

#################
# QT MOC
#################

FIND_FILE([moc],[$with_apps $QT_TOOLS_PATHS],
[
	QT_MOC_DIR="$FILE_DIR"
],[
	AC_MSG_ERROR([moc not found - please install qt-dev-tools])
])
AC_SUBST(QT_MOC_DIR)

#################
# QT LRELEASE
#################

FIND_FILE([lrelease],[$with_apps $QT_TOOLS_PATHS],
[
	QT_LRELEASE_DIR="$FILE_DIR"
],[
	AC_MSG_ERROR([lrelease not found - please install qt-dev-tools])
])
AC_SUBST(QT_LRELEASE_DIR)

########################################################################

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h limits.h netinet/in.h stdlib.h string.h sys/ioctl.h sys/socket.h unistd.h])

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

AX_CREATE_STDINT_H([kadu-stdint.h])

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
	save_CXXFLAGS="$CXXFLAGS"
	FIND_HEADER([libgadu.h],[$with_existing_libgadu $with_existing_libgadu/include],
	[
		GADU_INCLUDES="-I$FILE_DIR"
	])
	GADU_LIBS=-lgadu
	FIND_LIBRARY([gadu],[$with_existing_libgadu $with_existing_libgadu/lib],
	[
		GADU_LIBS="$GADU_LIBS -L$FILE_DIR"
	])
	CXXFLAGS="$CXXFLAGS $GADU_INCLUDES"
	AC_CHECK_MEMBER([struct gg_dcc7.uin], [],
	[AC_MSG_ERROR([Your libgadu doesn't support for dcc7, please use our SVN snapshot by removing --with-existing-libgadu option !])],
	[#include <libgadu.h>])
	CXXFLAGS="$save_CXXFLAGS"
else
	MY_SUBDIRS="$MY_SUBDIRS libgadu"
	GADU_LIBS="../libgadu/src/.libs/libgadu.a"
	GADU_INCLUDES="-I../libgadu/include"
fi
AC_SUBST(GADU_LIBS)
AC_SUBST(GADU_INCLUDES)

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
	CFLAGS="$CFLAGS -ggdb"
	libgadu_CFLAGS="$CFLAGS -ggdb"
else
	CXXFLAGS="$CXXFLAGS -s"
	CFLAGS="$CFLAGS -s"
	libgadu_CFLAGS="$CFLAGS -s"
fi


# macosx support
# needs to be done better
AC_ARG_ENABLE(macosx,[  --enable-macosx         turn on some MacOSX compiler flags])
if test -z "$enable_macosx"; then
	enable_macosx=no
	DYNAMIC_OPT="-rdynamic"
	SO_EXT="so"
	SHARED_OPT="-shared"
	SHARED_FLAGS=
fi
if test "$enable_macosx" = "yes"; then
	# -s kills dynamic modules
	CXXFLAGS="`echo $CXXFLAGS | sed 's/ -s//'`"
	CFLAGS="`echo $CFLAGS | sed 's/ -s//'`"

	LDFLAGS="$LDFLAGS -framework Carbon -framework QuickTime -lz -framework OpenGL -framework AGL"
	DYNAMIC_OPT="-dynamic"
	SO_EXT="dylib"
	SHARED_OPT="-dynamiclib"
	SHARED_FLAGS="-undefined suppress -flat_namespace"

#only on MacOSX>=10.3
#	SHARED_FLAGS="-undefined dynamic_lookup"
fi
AC_SUBST(DYNAMIC_OPT)
AC_SUBST(SO_EXT)
AC_SUBST(SHARED_OPT)
AC_SUBST(SHARED_FLAGS)


# signal handling support
AC_ARG_ENABLE(sig-handling,[  --disable-sig-handling  disable system signals handling])
if test -z "$enable_sig_handling"; then
	enable_sig_handling=yes
fi
if test "$enable_sig_handling" = "yes"; then
	AC_DEFINE(SIG_HANDLING_ENABLED,1,[Define if you want system signals handling])
fi

# precompiled headers support
AC_ARG_ENABLE(pheaders,[  --enable-pheaders       enable precompiled headers support])
if test -z "$enable_pheaders"; then
	enable_pheaders=no
fi
if test "$enable_pheaders" = "yes"; then
	rm -f kadu-core/kadu-headers.h2
	for i in kadu-config.h `ls -1 kadu-core | grep "\.h$" | grep -v "kadu-headers.h"`; do
		echo "#include \"$i\"" >> kadu-core/kadu-headers.h2
	done
	grep -h '#include <q[^>]*>' kadu-core/*.cpp | sort -u >> kadu-core/kadu-headers.h2
	if test -f kadu-core/kadu-headers.h; then
		diff kadu-core/kadu-headers.h kadu-core/kadu-headers.h2 > /dev/null
		# if files differ
		if test $? != 0 ; then
			mv kadu-core/kadu-headers.h2 kadu-core/kadu-headers.h
		else
			rm kadu-core/kadu-headers.h2
		fi
	else
		mv kadu-core/kadu-headers.h2 kadu-core/kadu-headers.h
	fi
else
	if test ! -f kadu-core/kadu-headers.h; then
		touch kadu-core/kadu-headers.h
	fi
fi
AC_SUBST(enable_pheaders)
AM_CONDITIONAL(PHEADERS_ENABLED,test "$enable_pheaders" = "yes")

# dependency tracking support
AC_ARG_ENABLE(dependency-tracking,[  --disable-dependency-tracking speeds up one-time builds])
if test -z "$enable_dependency_tracking"; then
	enable_dependency_tracking=yes
fi
AC_SUBST(enable_dependency_tracking)
#AM_CONDITIONAL(DEPENDENCY_TRACKING_DISABLED,test "$disable_dependency_tracking" = "yes")

# final build support
# AC_ARG_ENABLE(final,[  --enable-final           speeds up one-time builds])
# if test -z "$enable_final"; then
enable_final=no
# fi
# AC_SUBST(enable_final)

# dist-info support
AC_ARG_ENABLE(dist-info,[  --enable-dist-info=DIST enable distribution type information, default: sources])
if test -z "$enable_dist_info"; then
	enable_dist_info=sources
fi
AC_DEFINE_UNQUOTED(DIST_TYPE,"$enable_dist_info",[Define distribution type of this copy of application])

# autodownload support
AC_ARG_ENABLE(autodownload,[  --disable-autodownload  disable automatic download of modules/icons/emoticons])
if test -z "$enable_autodownload"; then
	enable_autodownload=yes
fi

if test -f SNAPSHOT; then
	snapshot=yes
else
	snapshot=no
fi
AC_SUBST(snapshot)

# OpenSSL support
# Maybe it will be back when GG servers have SSL support finally
#AC_CHECK_OPENSSL
#if test -z "$have_openssl"; then
#	have_openssl=no
#fi
#AM_CONDITIONAL(OPENSSL,[test "$have_openssl" = "yes"])
#AC_SUBST(OPENSSL_LIBS)
#AC_SUBST(OPENSSL_INCLUDES)

# Loadable modules support
FIND_AVAILABLE_MODULES()
CHECK_MODULES_CONFIGURATION()
PROCESS_MODULE_SPEC_FILES()
CHECK_MODULES_DEPENDENCIES()
GENERATE_STATIC_MODULES_CODE()
AC_SUBST(shared_modules)
AC_SUBST(static_modules)
AC_SUBST(static_modules_ldflags)
AC_SUBST(STATIC_MODULES_DECLS)
AC_SUBST(STATIC_MODULES_REGRS)

# Icon themes support
FIND_AVAILABLE_ICON_THEMES()
CHECK_ICON_THEMES_CONFIGURATION()
PROCESS_ICON_THEMES()
AC_SUBST(enabled_icon_themes)

# Emoticon themes support
FIND_AVAILABLE_EMOTICON_THEMES()
CHECK_EMOTICON_THEMES_CONFIGURATION()
PROCESS_EMOTICON_THEMES()
AC_SUBST(enabled_emoticon_themes)

#
MY_SUBDIRS="$MY_SUBDIRS modules kadu-core translations varia"
AC_SUBST(MY_SUBDIRS)


########################################################################
# configure libgadu
########################################################################

echo
echo " ******************************************"
echo " * Configuring libgadu, please wait . . . *"
echo " ******************************************"
echo

# Maybe it will be back when GG servers have SSL support finally
ac_configure_args="$ac_configure_args --without-openssl"

if test "$with_existing_libgadu" = "no"; then
	ac_configure_args="$ac_configure_args 'CFLAGS_LIBGADU=$libgadu_CFLAGS' --enable-static --disable-shared"
	AC_CONFIG_SUBDIRS([libgadu])
fi

########################################################################
# finalize configure
########################################################################

AC_CONFIG_FILES([Makefile kadu-core/Makefile translations/Makefile varia/Makefile modules/Makefile varia/scripts/kadu-config kadu-core/modules_static.cpp])
AC_OUTPUT

#if test ! -f kadu-core/deps/about.d; then
	echo -n creating dependency tracking files...
	for i in kadu-core/*.h; do
		f="kadu-core/deps/`echo $i | sed 's/^kadu-core\/\(.*\).h$/\1/'`"
		touch ${f}_moc.d ${f}.d
	done
	touch kadu-core/deps/main.d
	echo done
#fi

if test -f kadu-config.h; then
	diff kadu-config.h kadu-config.tmp > /dev/null
	if test $? != 0 ; then
		cp kadu-config.tmp kadu-config.h
	fi
else
	cp kadu-config.tmp kadu-config.h
fi

rm -f logfile.txt

echo
echo " **************************************"
echo " * Kadu configuration is now complete *"
echo " **************************************"
echo
echo " Kadu $version was configured using options specified below:"
echo
echo "   Installation prefix:                [[$prefix]]"
echo "   Use pthread resolving in libgadu:   $with_pthread"
echo "   Compile with debug symbols:         $enable_debug"
echo "   System signals handling:            $enable_sig_handling"
echo "   Precompiled headers support:        $enable_pheaders"
echo "   Dependency tracking:                $enable_dependency_tracking"
echo "   Final build:                        $enable_final"
echo "   Link with existing libgadu:         $with_existing_libgadu"
# Maybe it will be back when GG servers have SSL support finally
#echo "   OpenSSL encryption support:         $have_openssl"
echo "   Distribution type information:      $enable_dist_info"
echo "   Additional c++ compiler flags:      $user_cxxflags"
echo "   Additional c   compiler flags:      $user_cflags"
echo
echo "   Static modules:   $static_modules"
echo "   Shared modules:   $shared_modules"
echo "   Disabled modules: $disabled_modules"
echo
echo "   Enabled icon themes:  $enabled_icon_themes"
echo "   Disabled icon themes: $disabled_icon_themes"
echo
echo "   Enabled emoticon themes:  $enabled_emoticon_themes"
echo "   Disabled emoticon themes: $disabled_emoticon_themes"
echo
echo " Run make now (Linux) or gmake (FreeBSD and others) to compile Kadu."
echo
