#!/usr/bin/env bash

type=$1
shift

result=""

source ../../../.config
for icon_theme in $*; do
	echo "option=\$icons_$icon_theme" > .temp
	source .temp

	if [ -z "$option" ]; then
		option="y"
	fi

	if [ "$type" == "$option" ]; then
		if [ -z "$result" ]; then
			result="$icon_theme"
		else
			result="$result;$icon_theme"
		fi
	fi
done

rm -f .temp

if [ ! -z "$result" ]; then
	echo $result
fi
