#!/bin/sh
if test "$*"; then
  ARGS="$*"
else
  if test -f config.log; then
    ARGS=`grep '^  \$ \./configure ' config.log | sed 's/^  \$ \.\/configure //' 2> /dev/null`
  fi
fi
aclocal -I m4
autoheader
autoconf
test x$NOCONFIGURE = x && echo "Running ./configure $ARGS" && ./configure $ARGS
