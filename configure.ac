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

# $1 - action if successed
# $2 - action if failed
AC_DEFUN(TRY_LINK_EXECINFO,[
	AC_TRY_LINK(
	[
		#include <execinfo.h>
	],[
			void* data;
			backtrace(&data, 100);
	],[
		$1
	],[
		$2
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
		if test -f $dir/$1; then
			FILE_DIR=$dir
			AC_MSG_RESULT([found in $FILE_DIR])
			$3
			break
		fi
	done
	if test -z "$FILE_DIR"; then
		AC_MSG_RESULT([not found])
		m4_if($4,,AC_MSG_ERROR([$1 not found!]))
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
		FIND_FILE([$1],[$with_includes $2],[$3],[$4])
	])
])

# $1 - library name (without "lib" prefix and extension)
# $2 - directories
# $3 - action if found but not in system dirs
# $4 - action if not found (default is error message)
# $5 - additional flags
AC_DEFUN(FIND_LIBRARY,[
	AC_MSG_CHECKING([if lib$1 is in system directories])
	save_CXXFLAGS="$CXXFLAGS"
	CXXFLAGS="$CXXFLAGS $5 -l$1"
	AC_TRY_LINK([],
	[
		return 0;
	],[
		CXXFLAGS="$save_CXXFLAGS"
		AC_MSG_RESULT([yes])
	],[
		CXXFLAGS="$save_CXXFLAGS"
		AC_MSG_RESULT([no])

		AC_MSG_CHECKING([if lib$1 is in specified directories: $with_libs $2])
		FILE_DIR=
		for dir in $with_libs $2
		do
			CXXFLAGS="$save_CXXFLAGS $5 -L$dir -l$1"
			AC_TRY_LINK([],
			[
				return 0;
			],[
				CXXFLAGS="$save_CXXFLAGS"
				FILE_DIR=$dir
				AC_MSG_RESULT([found in $FILE_DIR])
				$3
				break
			],[
				CXXFLAGS="$save_CXXFLAGS"
			])
		done
		if test -z "$FILE_DIR";
		then
			AC_MSG_RESULT([not found])
			m4_if($4,,AC_MSG_ERROR([lib$1 not found!]))
			$4
		fi
	])
])

# $1 - tool name
# $2 - action if not found (default is error message)
AC_DEFUN(FIND_TOOL,[
	AC_MSG_CHECKING([if $1 tool is available])
	TOOL_DIRS=
	tmp_TOOL_DIR=
	for dir in $with_apps `echo $PATH | sed "s/:/ /g"` 
	do
		if test -x $dir/$1; then
			tmp_TOOL_DIR=$dir
			TOOL_DIRS="$TOOL_DIRS $tmp_TOOL_DIR"
			AC_MSG_RESULT([found in $tmp_TOOL_DIR])
			break
		fi
	done
	if test -z "$tmp_TOOL_DIR"; then
		AC_MSG_RESULT([not found])
		m4_if($2,,AC_MSG_ERROR([$1 not found!]))
		$2
	fi	
])

AC_DEFUN(FIND_AVAILABLE_MODULES,[
	available_modules=
	cd modules
	for dir in *; do \
		if test -f $dir/spec; then \
			available_modules="$available_modules $dir"; \
		fi; \
	done
	for file in *.web; do \
		dir=`basename $file .web`; \
		if test ! -f $dir/spec; then \
			available_modules="$available_modules $dir"; \
		fi; \
	done
	cd ..
])

AC_DEFUN(FIND_AVAILABLE_ICON_THEMES,[
	available_icon_themes=
	cd varia/themes/icons
	for dir in *; do \
		if test -f $dir/icons.conf; then \
			available_icon_themes="$available_icon_themes $dir"; \
		fi; \
	done
	for file in *.web; do \
		dir=`basename $file .web`; \
		if test ! -f $dir/icons.conf; then \
			available_icon_themes="$available_icon_themes $dir"; \
		fi; \
	done
	cd ../../..
])

AC_DEFUN(FIND_AVAILABLE_EMOTICON_THEMES,[
	available_emoticon_themes=
	cd varia/themes/emoticons
	for dir in *; do \
		if test -f $dir/1/emots.txt; then \
			available_emoticon_themes="$available_emoticon_themes $dir"; \
		elif test -f $dir/emots.txt; then \
			available_emoticon_themes="$available_emoticon_themes $dir"; \
		fi; \
	done
	for file in *.web; do \
		dir=`basename $file .web`; \
		if test ! -f $dir/1/emots.txt; then \
			if test ! -f $dir/emots.txt; then \
				available_emoticon_themes="$available_emoticon_themes $dir"; \
			fi; \
		fi; \
	done
	cd ../../..
])

AC_DEFUN(CHECK_MODULES_CONFIGURATION,[
	static_modules=
	shared_modules=
	disabled_modules=
	. ./.config; \
	for mod in $available_modules; do \
		echo "option=\$module_$mod" > .temp; \
		. ./.temp; \
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

AC_DEFUN(CHECK_ICON_THEMES_CONFIGURATION,[
	enabled_icon_themes=
	disabled_icon_themes=
	. ./.config; \
	for i_t in $available_icon_themes; do \
		echo "option=\$icons_$i_t" > .temp; \
		. ./.temp; \
		if test -z "$option"; then \
			option=y; \
		fi; \
		case "$option" in
			y) enabled_icon_themes="$enabled_icon_themes $i_t";; \
			*) disabled_icon_themes="$disabled_icon_themes $i_t";; \
		esac; \
	done; \
	rm .temp
])

AC_DEFUN(CHECK_EMOTICON_THEMES_CONFIGURATION,[
	enabled_emoticon_themes=
	disabled_emoticon_themes=
	. ./.config; \
	for e_t in $available_emoticon_themes; do \
		echo "option=\$emoticons_$e_t" > .temp; \
		. ./.temp; \
		if test -z "$option"; then \
			option=y; \
		fi; \
		case "$option" in
			y) enabled_emoticon_themes="$enabled_emoticon_themes $e_t";; \
			*) disabled_emoticon_themes="$disabled_emoticon_themes $e_t";; \
		esac; \
	done; \
	rm .temp
])

AC_DEFUN(PROCESS_MODULE_SPEC_FILES,[
	echo
	echo " ******************************************"
	echo " * Configuring modules, please wait . . . *"
	echo " ******************************************"
	langs="pl en de fr"
	static_modules_ldflags=
	static_modules_cxxflags=
	for mod in $static_modules $shared_modules; do \
		if test -f modules/$mod.web; then \
			if test -f modules/$mod/.autodownloaded -o ! -f modules/$mod/spec; then \
				cd modules; \
				if ! ../varia/scripts/autodownload $mod module; then \
					AC_MSG_ERROR([$mod download failed!]); \
				fi; \
				cd ..; \
			fi; \
		fi; \
		if test -f modules/$mod/spec; then \
			echo; \

			echo ">>> Configuring $mod module..."; \
			MODULE_LIBS= && \
			MODULE_LIBS_PATH= && \
			MODULE_INCLUDES= && \
			MODULE_INCLUDES_PATH= && \
			MODULE_TOOLS= && \
			MODULE_CONFIGURE_CMD= && \
			MODULE_CXXFLAGS= && \
			MODULE_LDFLAGS= && \
			MODULE_DIR=../modules/$mod && \
			. ./modules/$mod/spec && \
			echo "MODULE_LIBS:           $MODULE_LIBS" && \
			echo "MODULE_LIBS_PATH:      $MODULE_LIBS_PATH" && \
			echo "MODULE_INCLUDES:       $MODULE_INCLUDES" && \
			echo "MODULE_INCLUDES_PATH:  $MODULE_INCLUDES_PATH" && \
			echo "MODULE_CXXFLAGS:       $MODULE_CXXFLAGS" && \
			echo "MODULE_LDFLAGS:        $MODULE_LDFLAGS" && \
			echo "MODULE_TOOLS:          $MODULE_TOOLS" && \
			echo "MODULE_CONFIGURE_CMD:  $MODULE_CONFIGURE_CMD" && \
			echo -e "all:" > modules/$mod/Makefile && \
			echo -e "\tmake -C .. module_$mod\n" >> modules/$mod/Makefile && \
			echo -e "install:" >> modules/$mod/Makefile && \
			echo -e "\tmake -C .. module_$mod install_$mod\n" >> modules/$mod/Makefile && \
			echo -e "clean:" >> modules/$mod/Makefile && \
			echo -e "\trm -f *.o *.d *.so *.dylib *.a *_moc.cpp .final.d .final.cpp.o .final.c.o\n" >> modules/$mod/Makefile && \
			echo -e "distclean: clean" >> modules/$mod/Makefile && \
			echo -e "\trm -f .cxxflags .ldflags .final.cpp .final.c translations/*.qm Makefile\n" >> modules/$mod/Makefile && \
			echo -e "update-translations:" >> modules/$mod/Makefile && \
			for i in $langs; do \
				echo -e "\tlupdate ${MODULE_SOURCES} -ts translations/${mod}_${i}.ts" >> modules/$mod/Makefile; \
			done && \
			
			if test $enable_final == yes; then \
				for src in $MODULE_SOURCES; do \
					if test "`echo $src | sed 's/.*\.\(.*\)$/\1/'`" == "cpp"; then \
						echo "#include \"$src\"" >> modules/$mod/.final2.cpp; \
					else
						echo "#include \"$src\"" >> modules/$mod/.final2.c; \
					fi \
				done && \
				for hdr in $MODULE_MOC_HEADERS; do \
					moc=`echo $hdr | sed "s/.h$/_moc.cpp/"`; \
					echo "#include \"$moc\"" >> modules/$mod/.final2.cpp; \
				done && \
				if test -f modules/$mod/.final.cpp; then \
					diff modules/$mod/.final.cpp modules/$mod/.final2.cpp >/dev/null; \
					if test $? != 0 ; then \
						mv modules/$mod/.final2.cpp modules/$mod/.final.cpp; \
					else \
						rm modules/$mod/.final2.cpp; \
					fi \
				else \
					mv modules/$mod/.final2.cpp modules/$mod/.final.cpp; \
				fi && \
				if test -f modules/$mod/.final.c; then \
					diff modules/$mod/.final.c modules/$mod/.final2.c >/dev/null; \
					if test $? != 0 ; then \
						mv modules/$mod/.final2.c modules/$mod/.final.c; \
					else \
						rm modules/$mod/.final2.c; \
					fi \
				else \
					if test -f modules/$mod/.final2.c; then \
						mv modules/$mod/.final2.c modules/$mod/.final.c; \
					fi \
				fi \
			else \
				rm -f modules/$mod/.final.cpp; \
				rm -f modules/$mod/.final.c; \
			fi && \
			
			for lib in $MODULE_LIBS; do \
				FIND_LIBRARY([$lib],[$MODULE_LIBS_PATH],
				[
					MODULE_LDFLAGS="$MODULE_LDFLAGS -L$FILE_DIR"
				],,
				[$MODULE_LDFLAGS]) \
				MODULE_LDFLAGS="$MODULE_LDFLAGS -l$lib"; \
			done && \
			for hdr in $MODULE_INCLUDES; do \
				FIND_HEADER([$hdr],[$MODULE_INCLUDES_PATH],
				[
					MODULE_CXXFLAGS="$MODULE_CXXFLAGS -I$FILE_DIR"
				]) \
			done && \
			for tool in $MODULE_TOOLS; do \
				FIND_TOOL([$tool]) \
			done && \
			if test -n "$MODULE_CONFIGURE_CMD"; then \
				if eval "$MODULE_CONFIGURE_CMD"; then \
					echo "Additional module configuration command successed."; \
				else \
					AC_MSG_ERROR([Additional module configuration command failed!]); \
				fi; \
			fi && \
			
			#removing double spaces from variables
			while test `echo "$MODULE_CXXFLAGS" | grep "  " | wc -l` -ge 1 ; do \
				MODULE_CXXFLAGS=`echo "$MODULE_CXXFLAGS" | sed "s/  / /g"`; \
			done && \
			while test `echo "$MODULE_LDFLAGS" | grep "  " | wc -l` -ge 1 ; do \
				MODULE_LDFLAGS=`echo "$MODULE_LDFLAGS" | sed "s/  / /g"`; \
			done && \
			if test `echo "$MODULE_CXXFLAGS" | grep "^ .*" | wc -l` -ge 1; then \
				MODULE_CXXFLAGS=`echo "$MODULE_CXXFLAGS" | sed "s/^ //"`; \
			fi && \
			if test `echo "$MODULE_LDFLAGS" | grep "^ .*" | wc -l` -ge 1; then \
				MODULE_LDFLAGS=`echo "$MODULE_LDFLAGS" | sed "s/^ //"`; \
			fi && \

			echo -n "$MODULE_CXXFLAGS" > modules/$mod/.cxxflags && \
			if test -z `expr " $shared_modules " : ".* \($mod\) "`; then \
				echo -n > modules/$mod/.ldflags; \
				static_modules_ldflags="$static_modules_ldflags $MODULE_LDFLAGS"; \
			else \
				echo -n "$MODULE_LDFLAGS" > modules/$mod/.ldflags; \
			fi || exit 1; \
		fi; \
	done
])

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
	done
])

AC_DEFUN(PROCESS_EMOTICON_THEMES,[
	echo
	echo " **************************************************"
	echo " * Configuring emoticon themes, please wait . . . *"
	echo " **************************************************"
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
	done
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

AC_CONFIG_SRCDIR([kadu/kadu.cpp])
AC_CONFIG_AUX_DIR([admin])
AM_CONFIG_HEADER([kadu-config.tmp:kadu-config.h.in])
AM_MAINTAINER_MODE
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE([no-dependencies dist-bzip2])

########################################################################
# check for programs, libraries and functions
########################################################################

user_cxxflags="$CXXFLAGS"
CXXFLAGS="-fno-exceptions -pipe -Wall $user_cxxflags"

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
QT_LIBS_PATHS="    $QTDIR/lib     /usr/lib/qt3/lib     /usr/lib/qt/lib     /usr/local/qt/lib     /usr/local/lib     /usr/lib      /usr/lib/qt     /usr/share/qt3/lib     /usr/X11R6/lib     /sw/lib        /usr/lib/qt3     /usr/X11R6/lib/qt     /usr/X11R6/lib/qt3     /usr/lib/qt3/lib64  $QTDIR/lib64"
QT_INCLUDES_PATHS="$QTDIR/include /usr/lib/qt3/include /usr/lib/qt/include /usr/local/qt/include /usr/local/include /usr/include  /usr/include/qt /usr/share/qt3/include /usr/X11R6/include /sw/include/qt /usr/include/qt3 /usr/X11R6/include/qt /usr/X11R6/include/qt3"
QT_TOOLS_PATHS="   $QTDIR/bin     /usr/lib/qt3/bin     /usr/lib/qt/bin     /usr/local/qt/bin     /usr/local/bin     /usr/bin      /bin            /usr/share/qt3/bin     /usr/X11R6/bin"
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

FIND_HEADER([qwidget.h],[$QT_INCLUDES_PATHS],
[
	QT_INCLUDES="-I$FILE_DIR"
],[
	AC_MSG_ERROR([libqt-mt headers not found - please install libqt3-devel (>=3.2 <4.0) - check: http://kadu.net/qt])
])
AC_SUBST(QT_INCLUDES)

#################
# QT Library
#################


QT_LIBS="-lqt-mt"
FIND_LIBRARY([qt-mt],[$QT_LIBS_PATHS],
[
	QT_LIBS="$QT_LIBS -L$FILE_DIR"
],[
	AC_MSG_ERROR([libqt-mt not found - please install libqt3 (>=3.2 <4.0) - check: http://kadu.net/qt])
],[$PNG_LIBS $PTHREAD_LDFLAGS $PTHREAD_LIBS])
AC_SUBST(QT_LIBS)


AC_MSG_CHECKING(for Qt version)
save_CXXFLAGS="$CXXFLAGS"
CXXFLAGS="$QT_INCLUDES"
AC_TRY_LINK(
[
	#include <qglobal.h>
],[
	#if QT_VERSION < 0x030200
		#error
	#endif
	#if QT_VERSION >= 0x040000
		#error
	#endif
],
[
	AC_MSG_RESULT(ok)
	
],[
	AC_MSG_RESULT(bad)
	AC_MSG_ERROR([Your Qt version is not appropiate! Get Qt >= 3.2 and < 4.0])
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
	libgadu_CFLAGS="-ggdb"
else
	CXXFLAGS="$CXXFLAGS -s"
	CFLAGS="$CFLAGS -s"
	libgadu_CFLAGS="-DGG_DEBUG_DISABLE -s"
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
	rm -f kadu/kadu-headers.h2
	for i in kadu-config.h `ls -1 kadu | grep "\.h$" | grep -v "kadu-headers.h" | grep -v "pending_events.h"`; do
		echo "#include \"$i\"" >> kadu/kadu-headers.h2
	done
	grep -h '#include <q[^>]*>' kadu/*.cpp | sort -u >> kadu/kadu-headers.h2
	if test -f kadu/kadu-headers.h; then
		diff kadu/kadu-headers.h kadu/kadu-headers.h2 > /dev/null
		# if files differ
		if test $? != 0 ; then
			mv kadu/kadu-headers.h2 kadu/kadu-headers.h
		else
			rm kadu/kadu-headers.h2
		fi
	else
		mv kadu/kadu-headers.h2 kadu/kadu-headers.h
	fi
else
	if test ! -f kadu/kadu-headers.h; then
		touch kadu/kadu-headers.h
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
AC_ARG_ENABLE(final,[  --enable-final           speeds up one-time builds])
if test -z "$enable_final"; then
	enable_final=no
fi
AC_SUBST(enable_final)

# dist-info support
AC_ARG_ENABLE(dist-info,[  --enable-dist-info=DIST enable distribution type information, default: sources])
if test -z "$enable_dist_info"; then
	enable_dist_info=sources
fi
AC_DEFINE_UNQUOTED(DIST_TYPE,"$enable_dist_info",[Define distribution type of this copy of application])

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
MY_SUBDIRS="$MY_SUBDIRS modules kadu translations varia"
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

AC_CONFIG_FILES([Makefile kadu/Makefile translations/Makefile varia/Makefile modules/Makefile varia/scripts/kadu-config kadu/modules_static.cpp])
AC_OUTPUT

#if test ! -f kadu/deps/about.d; then
	echo -n creating dependency tracking files...
	for i in kadu/*.h; do
		f="kadu/deps/`echo $i | sed 's/^kadu\/\(.*\).h$/\1/'`"
		touch ${f}_moc.d ${f}.d
	done
	touch kadu/deps/main.d
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
