#!/bin/bash

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
		result="$result;$icon_theme"
	fi

done

rm -f .temp

echo $result