#!/usr/bin/env bash

available_icon_themes=""
for dir in *; do
	if [ -d $dir/kadu_icons ]; then
		available_icon_themes="$available_icon_themes $dir"
	fi
done

if [ ! -z "$available_icon_themes" ]; then
	echo $available_icon_themes
fi
