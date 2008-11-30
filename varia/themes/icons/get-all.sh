#!/bin/bash

available_icon_themes=""
for dir in *; do
	if [ -f "$dir/icons.conf" ]; then
		available_icon_themes="$available_icon_themes $dir"
	fi
done

if test "$1" == "ON"; then
	for f in `ls *.web`; do
		dir=`basename $f .web`
		if [ ! -f "$dir/icons.conf" ]; then
			available_icon_themes="$available_icon_themes $dir"
		fi
	done
fi

if [ ! -z "$available_icon_themes" ]; then
	echo $available_icon_themes
fi
