#!/usr/bin/env bash

available_sound_themes=""
for dir in *; do
	if [ -f "$dir/sound.conf" ]; then
		available_sound_themes="$available_sound_themes $dir"
	fi
done

if [ "$1" == "ON" ]; then
	for f in `ls *.web`; do
		dir=`basename $f .web`
		if [ ! -f "$dir/sound.conf" ]; then
			available_sound_themes="$available_sound_themes $dir"
		fi
	done 2>/dev/null
fi

if [ ! -z "$available_sound_themes" ]; then
	echo $available_sound_themes
fi
