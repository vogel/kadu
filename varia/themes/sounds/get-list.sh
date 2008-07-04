#!/bin/bash

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
		result="$result $sound_theme"
	fi

done

rm -f .temp

echo $result