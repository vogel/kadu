#!/usr/bin/env bash

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
		if [ -z "$result" ]; then
			result="$emoticon_theme"
		else
			result="$result;$emoticon_theme"
		fi
	fi
done

rm -f .temp

if [ ! -z "$result" ]; then
	echo $result
fi
