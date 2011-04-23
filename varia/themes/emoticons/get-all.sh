#!/usr/bin/env bash

available_emots_themes=""
for dir in *; do
	if [[ $(find ${dir} -name emots.txt) ]]; then
		available_emots_themes="$available_emots_themes $dir"
	fi
done

if [ ! -z "$available_emots_themes" ]; then
	echo $available_emots_themes
fi
