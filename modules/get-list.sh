#!/usr/bin/env bash

result=""

source ../.config
for module in $*; do
	echo "option=\$module_$module" > .temp
	source .temp

	if [ ! "$option" == "n" ]; then
		result="$result;$module"
	fi

done

rm -f .temp

echo $result