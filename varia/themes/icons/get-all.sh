#!/usr/bin/env bash

available_icon_themes=""
for dir in *; do
	if [ -f $dir/icons.conf ]; then
		available_icon_themes="$available_icon_themes $dir"
	fi
done

if [ "$1" == "ON" ]; then
	for file in *.web; do
		dir=`basename $file .web`
		if [ ! -f $dir/icons.conf ]; then
			available_icon_themes="$available_icon_themes $dir"
		fi
	done
fi

if [ ! -z "$available_icon_themes" ]; then
	echo $available_icon_themes
fi
