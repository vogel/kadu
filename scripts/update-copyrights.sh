#!/bin/bash

BASEDIR=${0%/*}

find kadu-core modules -name "*\.h" -o -name "*\.cpp" | xargs --max-arg=1 $BASEDIR/get-file-authors-and-dates.sh
