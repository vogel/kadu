#!/bin/sh
if test "$*"; then
	ARGS="$*"
else
	test -f config.log && ARGS=`grep '^  \$ \./configure ' config.log | sed 's/^  \$ \.\/configure //' 2> /dev/null`
fi
aclocal -I m4 || exit 1
autoheader || exit 1
autoconf || exit 1
test x$NOCONFIGURE = x && echo "Running ./configure $ARGS" && ./configure $ARGS

