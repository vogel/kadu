#!/bin/sh
if test "$*"; then
	ARGS="$*"
else
	test -f config.log && ARGS=`grep '^  \$ \./configure ' config.log | sed 's/^  \$ \.\/configure //' 2> /dev/null`
fi
aclocal -I m4
autoheader
autoconf
test x$NOCONFIGURE = x && echo "Running ./configure $ARGS" && ./configure $ARGS
