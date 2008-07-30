#!/bin/bash

available_icon_themes=""
for dir in *; do
	if [ -f $dir/icons.conf ]; then
		available_icon_themes="$available_icon_themes $dir"
	fi
done

if test "$1" == "ON"; then
	for file in *.web; do
		dir=`basename $file .web`
		if [ ! -f $dir/icons.conf ]; then
			available_icon_themes="$available_icon_themes $dir"
		fi
	done
fi

echo -n $available_icon_themes
