#!/bin/bash

available_sound_themes=""
for dir in *; do
	if [ -f $dir/sound.conf ]; then
		available_sound_themes="$available_sound_themes $dir"
	fi
done

if test "$1" == "ON"; then
	for file in *.web; do
		dir=`basename $file .web`
		if [ ! -f $dir/sound.conf ]; then
			available_sound_themes="$available_sound_themes $dir"
		fi
	done
fi

echo -n $available_sound_themes
