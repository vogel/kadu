#!/bin/bash

available_emots_themes=""
for dir in *; do
	if [[ $(find ${dir} -name emots.txt) ]]; then
		available_emots_themes="$available_emots_themes $dir"
	fi
done

if [ "$1" == "ON" ]; then
	for f in `ls *.web`; do
		dir=`basename $f .web`
		if [ ! -f "$dir/emots.txt" ]; then
			available_emots_themes="$available_emots_themes $dir"
		fi
	done
fi

if [ ! -z "$available_emots_themes" ]; then
	echo $available_emots_themes
fi
