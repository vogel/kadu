#!/bin/sh
if test $*; then
  ARGS="$*"
else
  if test -f config.log; then
    ARGS=`grep '^  \$ \./configure ' config.log | sed 's/^  \$ \.\/configure //' 2> /dev/null`
  fi
fi
aclocal -I m4
autoheader
autoconf
echo "Running ./configure $ARGS"
test x$NOCONFIGURE = x && ./configure $ARGS
