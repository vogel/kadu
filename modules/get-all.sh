#!/usr/bin/env bash

available_modules=""
for dir in *; do
	if [ -f $dir/$dir.desc ]; then
		available_modules="$available_modules;$dir"
	fi
done

echo $available_modules
