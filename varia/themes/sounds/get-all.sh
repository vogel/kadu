#!/usr/bin/env bash

available_sound_themes=""
for dir in *; do
	if [ -f "$dir/sound.conf" ]; then
		available_sound_themes="$available_sound_themes $dir"
	fi
done

if [ ! -z "$available_sound_themes" ]; then
	echo $available_sound_themes
fi
