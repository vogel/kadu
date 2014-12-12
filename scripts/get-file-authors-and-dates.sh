#!/usr/bin/env bash

BASEDIR=${0%/*}
FILE=$1

echo "Updating copyright data for $FILE"

AWK_EXTRACT=$BASEDIR/get-file-authors-and-dates.awk
AWK_INSERT=$BASEDIR/replace-copyright-block.awk

AUTHORS=`git --no-pager log --follow --date=short --pretty=format:"%ad%x09%an (%ae)" $FILE | cut -c1,2,3,4,11- | sort | uniq | awk -f $AWK_EXTRACT`
awk --assign AUTHORS="$AUTHORS" -f $AWK_INSERT $FILE > $FILE.copyright
mv $FILE.copyright $FILE
