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
