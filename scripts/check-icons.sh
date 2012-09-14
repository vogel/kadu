#!/usr/bin/env bash

echo "Wrong icons:"
allok=true

for s in 16x16 22x22 32x32 48x48 64x64 96x96 128x128; do
	for f in $(find varia/themes -type f -regex ".*/${s}/.*"); do
		if ! identify -format '%wx%h ' "${f}" | grep "${s}" > /dev/null; then
			echo -n "${f}: "
			identify -format '%wx%h ' "${f}"
			allok=false
		fi
	done
done

if $allok; then
	echo "All OK!"
fi
