#!/bin/sh
# $Id: autoclean.sh,v 1.4 2006-08-18 21:00:53 wojtekka Exp $

rm -rf \
	aclocal.m4 \
	autom4te.cache \
	compile \
	confdefs.h \
	config.* \
	configure \
	depcomp \
	install-sh \
	INSTALL \
	libtool \
	ltconfig \
	ltmain.sh \
	Makefile \
	Makefile.in \
	missing \
	mkinstalldirs \
	stamp* \
	stdint.h \
	src/Makefile \
	src/Makefile.in \
	src/.deps \
	src/.libs \
	src/*.o \
	src/*.lo \
	src/*.la \
	include/Makefile \
	include/Makefile.in \
	include/libgadu.h \
	include/stamp* \
	pkgconfig/Makefile \
	pkgconfig/Makefile.in \
	pkgconfig/libgadu.pc \
	pkgconfig/stamp*
