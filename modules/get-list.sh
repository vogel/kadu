#!/usr/bin/env bash

type=$1
shift

result=""

source ../.config
for module in $*; do
	echo "option=\$module_$module" > .temp
	source .temp

	if [ -z "$option" ]; then
		option="m"
	fi

	if [ "$type" == "$option" ]; then
		result="$result;$module"
	fi

done

rm -f .temp

echo $result