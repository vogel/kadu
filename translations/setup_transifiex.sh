#!/usr/bin/env bash

tx init

tx set --auto-local -r kadu.kadu  -s en 'translations/kadu_<lang>.ts' --execute

plugins=""
cd plugins
for dir in *; do
	if [ -d $dir ]; then
		plugins="$plugins $dir"
	fi
done

cd ..

for dir in $plugins; do
	dir1=$dir\_
	tx set --auto-local -r kadu.$dir  -s en "plugins/$dir/translations/$dir1<lang>.ts" --execute
done
