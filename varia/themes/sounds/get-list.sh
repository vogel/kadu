#!/usr/bin/env bash

type=$1
shift

result=""

source ../../../.config
for sound_theme in $*; do
	echo "option=\$sound_$sound_theme" > .temp
	source .temp

	if [ -z "$option" ]; then
		option="y"
	fi

	if [ "$type" == "$option" ]; then
		if [ -z "$result" ]; then
			result="$sound_theme"
		else
			result="$result;$sound_theme"
		fi
	fi
done

rm -f .temp

if [ ! -z "$result" ]; then
	echo $result
fi
