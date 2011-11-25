#!/bin/bash

BASEDIR=${0%/*}

find kadu-core plugins -name "*\.h" -o -name "*\.cpp" | grep -v 3rdparty | xargs --max-arg=1 $BASEDIR/sort-includes.sh
