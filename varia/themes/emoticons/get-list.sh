#!/bin/bash

type=$1
shift

result=""

source ../../../.config
for emoticon_theme in $*; do
	echo "option=\$emoticons_$emoticon_theme" > .temp
	source .temp

	if [ -z "$option" ]; then
		option="y"
	fi

	if [ "$type" == "$option" ]; then
		result="$result $emoticon_theme"
	fi

done

rm -f .temp

echo $result