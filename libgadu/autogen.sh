#!/bin/sh
aclocal -I m4
autoheader
autoconf
test x$NOCONFIGURE = x && ./configure $*
