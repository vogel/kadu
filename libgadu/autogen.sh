#!/bin/sh
aclocal -I m4
autoconf
test x$NOCONFIGURE = x && ./configure $*
