#!/bin/bash

available_emots_themes=""
for dir in *; do
	if [ -f $dir/emots.txt ]; then
		available_emots_themes="$available_emots_themes $dir"
	fi
done

if test "$1" == "ON"; then
	for file in *.web; do
		dir=`basename $file .web`
		if [ ! -f $dir/emots.txt ]; then
			available_emots_themes="$available_emots_themes $dir"
		fi
	done
fi

echo -n $available_emots_themes
