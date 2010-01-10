#!/bin/bash

BASEDIR=${0%/*}
FILE=$1

AWK_SCRIPT=$BASEDIR/get-file-authors-and-dates.awk

git --no-pager log --date=short --pretty=format:"%ad%x09%an (%ae)" $FILE | cut -c1,2,3,4,11- | sort | uniq | awk -f $AWK_SCRIPT
