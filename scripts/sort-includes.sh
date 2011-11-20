#!/bin/bash

BASEDIR=${0%/*}
FILE=$1

echo "Sorting includes for $FILE"

AWK_SORT=$BASEDIR/sort-includes.awk

awk -f $AWK_SORT $FILE > $FILE.sorted-includes
mv $FILE.sorted-includes $FILE
