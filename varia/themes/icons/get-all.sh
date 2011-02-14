#!/usr/bin/env bash

available_icon_themes=""
for dir in *; do
	if [ -d $dir/kadu_icons ]; then
		available_icon_themes="$available_icon_themes $dir"
	fi
done

# workaround #1960
#if [ "$1" == "ON" ]; then
#	for file in *.web; do
#		dir=`basename $file .web`
#		if [ ! -d $dir/kadu_icons ]; then
#			available_icon_themes="$available_icon_themes $dir"
#		fi
#	done
#fi

if [ ! -z "$available_icon_themes" ]; then
	echo $available_icon_themes
fi
